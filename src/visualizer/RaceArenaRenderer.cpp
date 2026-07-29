#include "../../include/visualizer/RaceArenaRenderer.hpp"
#include "../../include/visualizer/FontRenderer.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

namespace Engine {

RaceArenaRenderer::RaceArenaRenderer() : m_animTime(0.0f) {}

void RaceArenaRenderer::drawProgressBar(float x, float y, float w, float h, float percent, float r, float g, float b, const std::string& label) {
    // Progress Bar Background Container
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.08f, 0.12f, 0.20f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // Outer Border
    glColor4f(0.2f, 0.4f, 0.6f, 0.7f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // Filled Progress Bar
    float fillW = (w - 4.0f) * (std::min(100.0f, std::max(0.0f, percent)) / 100.0f);
    if (fillW > 0) {
        glColor4f(r, g, b, 0.85f);
        glBegin(GL_QUADS);
        glVertex2f(x + 2.0f, y + 2.0f);
        glVertex2f(x + 2.0f + fillW, y + 2.0f);
        glVertex2f(x + 2.0f + fillW, y + h - 2.0f);
        glVertex2f(x + 2.0f, y + h - 2.0f);
        glEnd();
    }

    // Label
    FontRenderer::getInstance().renderText(x + 8.0f, y + (h - 14.0f) / 2.0f, label, 0.65f, 1.0f, 1.0f, 1.0f, 0.95f);
}

void RaceArenaRenderer::drawSpeedometerGauge(float cx, float cy, float radius, float valPercent, float r, float g, float b, const std::string& title, const std::string& valueStr) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Dark Gauge Disk
    glColor4f(0.05f, 0.08f, 0.15f, 0.95f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 32; ++i) {
        float angle = 3.1415926f * (0.75f + 1.5f * i / 32.0f);
        glVertex2f(cx + std::cos(angle) * radius, cy + std::sin(angle) * radius);
    }
    glEnd();

    // Gauge Outer Rim
    glColor4f(r, g, b, 0.90f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 32; ++i) {
        float angle = 3.1415926f * (0.75f + 1.5f * i / 32.0f);
        glVertex2f(cx + std::cos(angle) * radius, cy + std::sin(angle) * radius);
    }
    glEnd();

    // Active Needle Pointer
    float needleAngle = 3.1415926f * (0.75f + 1.5f * (std::min(100.0f, std::max(0.0f, valPercent)) / 100.0f));
    glColor4f(1.0f, 0.9f, 0.2f, 1.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(cx, cy);
    glVertex2f(cx + std::cos(needleAngle) * (radius - 5.0f), cy + std::sin(needleAngle) * (radius - 5.0f));
    glEnd();

    // Center Hub Dot
    glColor4f(1.0f, 0.9f, 0.2f, 1.0f);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(cx, cy);
    glEnd();

    // Title ABOVE Disk (Cleanly separated)
    float titleW = title.length() * 6.5f;
    FontRenderer::getInstance().renderText(cx - (titleW / 2.0f), cy - radius - 16.0f, title, 0.65f, r, g, b, 0.95f);

    // Value String BELOW Disk (Cleanly separated)
    float valW = valueStr.length() * 6.5f;
    FontRenderer::getInstance().renderText(cx - (valW / 2.0f), cy + radius + 5.0f, valueStr, 0.65f, 1.0f, 1.0f, 1.0f, 0.98f);
}

void RaceArenaRenderer::drawFinishLine(float x, float y, float w, float h) {
    int rows = 6;
    int cols = 2;
    float cellW = w / cols;
    float cellH = h / rows;

    glDisable(GL_BLEND);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if ((r + c) % 2 == 0) glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            else glColor4f(0.1f, 0.1f, 0.1f, 1.0f);

            glBegin(GL_QUADS);
            glVertex2f(x + c * cellW, y + r * cellH);
            glVertex2f(x + (c + 1) * cellW, y + r * cellH);
            glVertex2f(x + (c + 1) * cellW, y + (r + 1) * cellH);
            glVertex2f(x + c * cellW, y + (r + 1) * cellH);
            glEnd();
        }
    }
}

void RaceArenaRenderer::render(const RaceEngine& raceEngine, int screenWidth, int screenHeight) {
    m_animTime += 0.05f;

    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const auto& seq = raceEngine.getSequentialTelemetry();
    const auto& par = raceEngine.getParallelTelemetry();

    // 1. Header Title Banner
    std::vector<std::string> headerLines = {
        "THREADRACE: REAL-TIME HEAD-TO-HEAD STRATEGY DUEL",
        "Evaluating Ackermann A(" + std::to_string(raceEngine.getM()) + ", " + std::to_string(raceEngine.getN()) + ") | Controls: [LEFT/RIGHT] Speed Pacing   [SPACE] Pause   [R] Restart"
    };
    FontRenderer::getInstance().renderCardHUD(15.0f, 15.0f, static_cast<float>(screenWidth) - 30.0f, 65.0f, headerLines);

    // 2. Dual Cockpits Side-by-Side
    float cockpitW = (static_cast<float>(screenWidth) - 45.0f) / 2.0f;
    float leftX = 15.0f;
    float rightX = 30.0f + cockpitW;
    float topY = 95.0f;
    float cockpitH = 160.0f;

    // --- Left Cockpit: Sequential Racer (Cyan Theme) ---
    std::vector<std::string> seqLines = {
        "RACER 1: SEQUENTIAL (SINGLE THREADED)",
        "TIME ELAPSED: " + std::to_string(seq.elapsedTimeSec).substr(0, 5) + " s   STEPS: " + std::to_string(seq.stepCount),
        "ACTIVE THREADS: 1 (MINIMAL OVERHEAD)",
        "MEMORY FOOTPRINT: " + std::to_string(seq.memoryKb).substr(0, 5) + " KB (Low Stack Space)",
        "TIME COMPLEXITY: HIGH (SLOWER)"
    };
    FontRenderer::getInstance().renderCardHUD(leftX, topY, cockpitW, cockpitH, seqLines);

    // --- Right Cockpit: Parallel Racer (Gold/Purple Theme) ---
    std::vector<std::string> parLines = {
        "RACER 2: PARALLEL (MULTI-THREADED POOL)",
        "TIME ELAPSED: " + std::to_string(par.elapsedTimeSec).substr(0, 5) + " s   STEPS: " + std::to_string(par.stepCount),
        "ACTIVE THREADS: " + std::to_string(par.activeThreads) + " WORKERS (HIGH PARALLELISM)",
        "MEMORY FOOTPRINT: " + std::to_string(par.memoryKb).substr(0, 5) + " KB (Thread + Stack Pool)",
        "TIME COMPLEXITY: LOW (ULTRA FAST)"
    };
    FontRenderer::getInstance().renderCardHUD(rightX, topY, cockpitW, cockpitH, parLines);

    // 3. Dedicated Speedometer Gauges on Right Side of Each Cockpit Card (Zero Overlap!)
    float gaugeRadius = 30.0f;
    float gauge1X = leftX + cockpitW - 55.0f;
    float gauge2X = rightX + cockpitW - 55.0f;
    float gaugeY = topY + 75.0f;

    float seqGaugeVal = std::min(100.0f, static_cast<float>(seq.stepsPerSec / 1.0f));
    float parGaugeVal = std::min(100.0f, static_cast<float>(par.stepsPerSec / 4.0f));

    std::string seqPaceStr = std::to_string(static_cast<int>(seq.stepsPerSec)) + " S/s";
    std::string parPaceStr = std::to_string(static_cast<int>(par.stepsPerSec)) + " S/s";

    drawSpeedometerGauge(gauge1X, gaugeY, gaugeRadius, seqGaugeVal, 0.0f, 0.85f, 1.0f, "PACE", seqPaceStr);
    drawSpeedometerGauge(gauge2X, gaugeY, gaugeRadius, parGaugeVal, 1.0f, 0.84f, 0.0f, "PACE", parPaceStr);

    // 4. Central Race Track & Finish Line Arena
    float trackY = topY + cockpitH + 20.0f;
    float trackW = static_cast<float>(screenWidth) - 80.0f;
    float finishX = leftX + trackW - 25.0f;

    // Draw Race Track Background Panel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.03f, 0.05f, 0.10f, 0.95f);
    glBegin(GL_QUADS);
    glVertex2f(leftX, trackY);
    glVertex2f(leftX + trackW + 15.0f, trackY);
    glVertex2f(leftX + trackW + 15.0f, trackY + 230.0f);
    glVertex2f(leftX, trackY + 230.0f);
    glEnd();

    // Race Track Border
    glColor4f(0.0f, 0.95f, 1.0f, 0.5f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(leftX, trackY);
    glVertex2f(leftX + trackW + 15.0f, trackY);
    glVertex2f(leftX + trackW + 15.0f, trackY + 230.0f);
    glVertex2f(leftX, trackY + 230.0f);
    glEnd();

    // Draw Track 1: Sequential Track (Cyan)
    float lane1Y = trackY + 40.0f;
    drawProgressBar(leftX + 15.0f, lane1Y, trackW - 40.0f, 40.0f, seq.progressPercent, 0.0f, 0.85f, 1.0f, "RACER 1: SEQUENTIAL (" + std::to_string(static_cast<int>(seq.progressPercent)) + "%)");

    // Sequential Racing Vehicle/Orb
    float seqCarX = leftX + 15.0f + (trackW - 40.0f) * (seq.progressPercent / 100.0f);
    glColor4f(0.0f, 1.0f, 0.9f, 1.0f);
    glPointSize(16.0f);
    glBegin(GL_POINTS);
    glVertex2f(seqCarX, lane1Y + 20.0f);
    glEnd();

    // Draw Track 2: Parallel Track (Gold)
    float lane2Y = trackY + 120.0f;
    drawProgressBar(leftX + 15.0f, lane2Y, trackW - 40.0f, 40.0f, par.progressPercent, 1.0f, 0.84f, 0.0f, "RACER 2: PARALLEL (" + std::to_string(static_cast<int>(par.progressPercent)) + "%)");

    // Parallel Racing Vehicle/Orb
    float parCarX = leftX + 15.0f + (trackW - 40.0f) * (par.progressPercent / 100.0f);
    glColor4f(1.0f, 0.9f, 0.1f, 1.0f);
    glPointSize(16.0f);
    glBegin(GL_POINTS);
    glVertex2f(parCarX, lane2Y + 20.0f);
    glEnd();

    // Draw Checkered Finish Line 🏁
    drawFinishLine(finishX, trackY + 25.0f, 20.0f, 150.0f);

    // 5. Winner Victory Podium & Trade-Off Demystifier Banner
    if (par.isCompleted) {
        float bannerY = trackY + 170.0f;
        float bannerW = trackW - 20.0f;

        double speedup = raceEngine.getWinnerSpeedup();
        std::vector<std::string> victoryLines = {
            "🏁 RACER 2 (PARALLEL WORKER POOL) WINS THE RACE! 🏆",
            "SPEEDUP: " + std::to_string(speedup).substr(0, 4) + "x FASTER  |  TRADE-OFF DEMYSTIFIED: Parallelism sacrifices Memory/Thread Space to minimize Time!"
        };
        FontRenderer::getInstance().renderCardHUD(leftX + 15.0f, bannerY, bannerW, 50.0f, victoryLines);
    }
}

}
