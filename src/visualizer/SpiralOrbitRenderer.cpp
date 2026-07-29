#include "../../include/visualizer/SpiralOrbitRenderer.hpp"
#include "../../include/visualizer/FontRenderer.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Engine {

SpiralOrbitRenderer::SpiralOrbitRenderer() 
    : m_rotationAngle(0.0f), m_lastStepCount(0), m_lastRootId(-1) {}

void SpiralOrbitRenderer::reset() {
    m_historyPoints.clear();
    m_lastStepCount = 0;
    m_lastRootId = -1;
}

void SpiralOrbitRenderer::render(const AckermannStackEngine& engine, int screenWidth, int screenHeight, float camX, float camY, float camZoom) {
    glViewport(0, 0, screenWidth, screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-screenWidth / 2.0, screenWidth / 2.0, screenHeight / 2.0, -screenHeight / 2.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(camZoom, camZoom, 1.0f);
    glTranslatef(camX, camY, 0.0f);

    m_rotationAngle += 0.012f;

    Metrics metrics = engine.getMetrics();
    int rootId = engine.getRootNodeId();
    const auto& nodes = engine.getNodes();
    const auto& stack = engine.getStack();

    // Reset history if execution restarted, reset, or node graph re-initialized
    if (metrics.stepCount == 0 || metrics.stepCount < m_lastStepCount || nodes.empty()) {
        reset();
        m_lastRootId = rootId;
    }

    // Accumulate all newly spawned nodes into cumulative history safely
    for (const auto& kv : nodes) {
        int nid = kv.first;
        bool alreadyInHistory = false;
        for (const auto& p : m_historyPoints) {
            if (p.nodeId == nid) {
                alreadyInHistory = true;
                break;
            }
        }
        if (!alreadyInHistory) {
            const auto& node = kv.second;
            StepOrbitPoint pt;
            pt.stepIndex = m_historyPoints.size();
            pt.nodeId = node.id;
            pt.m = node.m;
            pt.n = node.n;
            pt.depth = node.depth;
            pt.isResolved = (node.status == NodeStatus::RESOLVED);
            m_historyPoints.push_back(pt);
        }
    }
    m_lastStepCount = metrics.stepCount;

    // Update resolution status for cumulative history points
    for (auto& pt : m_historyPoints) {
        auto it = nodes.find(pt.nodeId);
        if (it != nodes.end()) {
            pt.isResolved = (it->second.status == NodeStatus::RESOLVED);
        }
    }

    // Golden ratio constant phi = (1 + sqrt(5)) / 2
    const float phi = 1.61803398875f;

    // 1. Draw Background Golden Spiral Logarithmic Reference Curve
    glColor4f(0.12f, 0.28f, 0.55f, 0.3f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 400; ++i) {
        float theta = static_cast<float>(i) * 0.1f;
        float r = 10.0f * std::pow(phi, theta * 0.14f);
        if (r > 1200.0f) break;
        float x = r * std::cos(theta + m_rotationAngle);
        float y = r * std::sin(theta + m_rotationAngle);
        glVertex2f(x, y);
    }
    glEnd();

    // 2. Draw CUMULATIVE Golden Ratio Constellation Line connecting ALL historical evaluation steps!
    if (m_historyPoints.size() >= 2) {
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < m_historyPoints.size(); ++i) {
            const auto& pt = m_historyPoints[i];
            float theta = i * 0.6180339f * 2.0f * M_PI + m_rotationAngle;
            float r = 22.0f * std::sqrt(static_cast<float>(i + 1)) + (pt.depth * 12.0f);
            float x = r * std::cos(theta);
            float y = r * std::sin(theta);

            float ratio = std::min(1.0f, static_cast<float>(pt.depth) / std::max(1.0f, static_cast<float>(metrics.maxDepth)));
            if (pt.isResolved) {
                glColor4f(0.0f, 1.0f, 0.53f, 0.75f); // Emerald green resolved trail line
            } else {
                glColor4f(0.0f + ratio * 1.0f, 0.95f - ratio * 0.5f, 1.0f - ratio * 0.8f, 0.85f);
            }
            glVertex2f(x, y);
        }
        glEnd();
    }

    // 3. Draw ALL CUMULATIVE Historical Step Orbit Points in Golden Ratio Space
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < m_historyPoints.size(); ++i) {
        const auto& pt = m_historyPoints[i];
        float theta = i * 0.6180339f * 2.0f * M_PI + m_rotationAngle;
        float r = 22.0f * std::sqrt(static_cast<float>(i + 1)) + (pt.depth * 12.0f);
        float x = r * std::cos(theta);
        float y = r * std::sin(theta);

        if (pt.isResolved) {
            glColor4f(0.0f, 1.0f, 0.53f, 0.95f); // Green resolved step
        } else if (pt.m == 0) {
            glColor4f(1.0f, 0.84f, 0.0f, 0.95f); // Gold base case step
        } else {
            glColor4f(0.0f, 0.95f, 1.0f, 0.9f);  // Cyan active step
        }
        glVertex2f(x, y);
    }
    glEnd();

    // 4. Highlight Currently Active Stack Nodes with glowing target halos in Golden Ratio Space
    for (const auto& frame : stack) {
        for (size_t i = 0; i < m_historyPoints.size(); ++i) {
            if (m_historyPoints[i].nodeId == frame.nodeId) {
                float theta = i * 0.6180339f * 2.0f * M_PI + m_rotationAngle;
                float r = 22.0f * std::sqrt(static_cast<float>(i + 1)) + (m_historyPoints[i].depth * 12.0f);
                float x = r * std::cos(theta);
                float y = r * std::sin(theta);

                // Active stack frame pulsing target ring
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glColor4f(1.0f, 0.2f, 0.6f, 0.85f);
                glLineWidth(2.5f);
                glBegin(GL_LINE_LOOP);
                for (int s = 0; s < 16; ++s) {
                    float a = 2.0f * M_PI * s / 16.0f;
                    glVertex2f(x + std::cos(a) * 14.0f, y + std::sin(a) * 14.0f);
                }
                glEnd();

                // Clean label next to active orbit node
                std::string label = "A(" + std::to_string(frame.m) + "," + std::to_string(frame.n) + ")";
                FontRenderer::getInstance().renderText(x + 16.0f, y - 6.0f, label, 0.65f, 1.0f, 1.0f, 1.0f, 0.95f);
            }
        }
    }
}

}
