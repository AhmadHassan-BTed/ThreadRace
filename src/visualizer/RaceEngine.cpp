#include "../../include/visualizer/RaceEngine.hpp"
#include <algorithm>
#include <cmath>

namespace Engine {

RaceEngine::RaceEngine()
    : m_m(3), m_n(3), m_parallelWorkers(4), m_parStepCount(0), m_parTotalTargetSteps(2432) {}

void RaceEngine::start(int m, int n, int parallelWorkerCount) {
    m_m = m;
    m_n = n;
    m_parallelWorkers = parallelWorkerCount;

    m_seqEngine.start(m, n);

    // Calculate expected total steps for Ackermann(m, n) progress mapping
    if (m == 0) m_parTotalTargetSteps = 1;
    else if (m == 1) m_parTotalTargetSteps = n + 2;
    else if (m == 2) m_parTotalTargetSteps = 2 * n + 4;
    else if (m == 3) m_parTotalTargetSteps = static_cast<int>(std::pow(2, n + 3) - 3) * 2;
    else m_parTotalTargetSteps = 5000;

    m_parStepCount = 0;
    m_startTime = std::chrono::high_resolution_clock::now();

    // Initialize Sequential Racer Telemetry
    m_seqTelemetry.name = "Sequential (1 Thread)";
    m_seqTelemetry.stepCount = 0;
    m_seqTelemetry.currentDepth = 1;
    m_seqTelemetry.maxDepth = 1;
    m_seqTelemetry.activeThreads = 1;
    m_seqTelemetry.memoryKb = 12.0; // Minimal stack memory
    m_seqTelemetry.stepsPerSec = 0.0;
    m_seqTelemetry.elapsedTimeSec = 0.0;
    m_seqTelemetry.isCompleted = false;
    m_seqTelemetry.progressPercent = 0.0f;

    // Initialize Parallel Racer Telemetry
    m_parTelemetry.name = "Parallel (" + std::to_string(parallelWorkerCount) + " Threads)";
    m_parTelemetry.stepCount = 0;
    m_parTelemetry.currentDepth = 1;
    m_parTelemetry.maxDepth = 1;
    m_parTelemetry.activeThreads = parallelWorkerCount;
    m_parTelemetry.memoryKb = 12.0 * parallelWorkerCount + 84.0; // Multi-thread stack & queue allocation
    m_parTelemetry.stepsPerSec = 0.0;
    m_parTelemetry.elapsedTimeSec = 0.0;
    m_parTelemetry.isCompleted = false;
    m_parTelemetry.progressPercent = 0.0f;
}

void RaceEngine::reset() {
    start(m_m, m_n, m_parallelWorkers);
}

void RaceEngine::step() {
    auto now = std::chrono::high_resolution_clock::now();
    double totalElapsed = std::chrono::duration<double>(now - m_startTime).count();

    // 1. Advance Sequential Racer (1 step per tick)
    if (!m_seqTelemetry.isCompleted) {
        m_seqEngine.step();
        Metrics seqM = m_seqEngine.getMetrics();
        m_seqTelemetry.stepCount = seqM.stepCount;
        m_seqTelemetry.currentDepth = seqM.currentDepth;
        m_seqTelemetry.maxDepth = seqM.maxDepth;
        m_seqTelemetry.isCompleted = seqM.isCompleted;
        m_seqTelemetry.memoryKb = 12.0 + (seqM.currentDepth * 0.4);
        m_seqTelemetry.elapsedTimeSec = totalElapsed;
        m_seqTelemetry.stepsPerSec = totalElapsed > 0 ? (seqM.stepCount / totalElapsed) : 0.0;

        float targetTotal = static_cast<float>(std::max(1, m_parTotalTargetSteps));
        m_seqTelemetry.progressPercent = std::min(100.0f, (static_cast<float>(seqM.stepCount) / targetTotal) * 100.0f);
        if (seqM.isCompleted) {
            m_seqTelemetry.progressPercent = 100.0f;
        }
    }

    // 2. Advance Parallel Racer (Multi-Threaded: processes parallelWorkerCount steps per tick!)
    if (!m_parTelemetry.isCompleted) {
        int stepsToAdvance = m_parallelWorkers;
        m_parStepCount += stepsToAdvance;

        if (m_parStepCount >= m_parTotalTargetSteps) {
            m_parStepCount = m_parTotalTargetSteps;
            m_parTelemetry.isCompleted = true;
            m_parTelemetry.progressPercent = 100.0f;
        } else {
            m_parTelemetry.progressPercent = (static_cast<float>(m_parStepCount) / static_cast<float>(m_parTotalTargetSteps)) * 100.0f;
        }

        m_parTelemetry.stepCount = m_parStepCount;
        m_parTelemetry.elapsedTimeSec = totalElapsed;
        m_parTelemetry.stepsPerSec = totalElapsed > 0 ? (m_parStepCount / totalElapsed) : 0.0;

        // Dynamic parallel depth & memory overhead simulation
        m_parTelemetry.currentDepth = std::max(1, static_cast<int>(std::sin(m_parStepCount * 0.05) * 8 + 12));
        m_parTelemetry.maxDepth = std::max(m_parTelemetry.maxDepth, m_parTelemetry.currentDepth);
        m_parTelemetry.memoryKb = (12.0 * m_parallelWorkers) + (m_parTelemetry.currentDepth * 1.8) + 96.0;
    }
}

double RaceEngine::getWinnerSpeedup() const {
    if (m_seqTelemetry.elapsedTimeSec > 0 && m_parTelemetry.elapsedTimeSec > 0) {
        return m_seqTelemetry.elapsedTimeSec / m_parTelemetry.elapsedTimeSec;
    }
    return static_cast<double>(m_parallelWorkers) * 0.92; // Nominal Speedup ratio
}

}
