#ifndef RACE_ENGINE_HPP
#define RACE_ENGINE_HPP

#include "AckermannStackEngine.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <cstdint>

namespace Engine {

struct RacerTelemetry {
    std::string name;
    int stepCount;
    int currentDepth;
    int maxDepth;
    int activeThreads;
    double memoryKb;
    double stepsPerSec;
    double elapsedTimeSec;
    bool isCompleted;
    float progressPercent;
};

class RaceEngine {
public:
    RaceEngine();

    void start(int m, int n, int parallelWorkerCount = 4);
    void step(uint32_t stepDelayMs, bool isPlaying);
    void reset();

    const AckermannStackEngine& getSequentialEngine() const { return m_seqEngine; }
    const RacerTelemetry& getSequentialTelemetry() const { return m_seqTelemetry; }
    const RacerTelemetry& getParallelTelemetry() const { return m_parTelemetry; }

    bool isRaceFinished() const { return m_seqTelemetry.isCompleted && m_parTelemetry.isCompleted; }
    int getM() const { return m_m; }
    int getN() const { return m_n; }
    double getWinnerSpeedup() const;

private:
    int m_m;
    int m_n;
    int m_parallelWorkers;

    AckermannStackEngine m_seqEngine;
    RacerTelemetry m_seqTelemetry;
    RacerTelemetry m_parTelemetry;

    int m_parStepCount;
    int m_parTotalTargetSteps;
    std::chrono::high_resolution_clock::time_point m_startTime;
};

}

#endif
