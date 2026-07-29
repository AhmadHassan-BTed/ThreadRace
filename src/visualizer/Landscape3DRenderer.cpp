#include "../../include/visualizer/Landscape3DRenderer.hpp"
#include "../../include/visualizer/FontRenderer.hpp"
#include <cmath>
#include <algorithm>

namespace Engine {

Landscape3DRenderer::Landscape3DRenderer() : m_rotationAngle(0.0f), m_initialized(false) {}

void Landscape3DRenderer::init() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_initialized = true;
}

void Landscape3DRenderer::render(const AckermannStackEngine& engine, int screenWidth, int screenHeight, float camX, float camY, float camZoom) {
    if (!m_initialized) init();

    glViewport(0, 0, screenWidth, screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // 3D perspective projection
    float fov = 60.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    float fH = tanf(fov / 360.0f * 3.1415926f) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_rotationAngle += 0.35f;

    // Global 3D Orbital camera scaling with camZoom and panning with camX/camY!
    float zDistance = -20.0f / camZoom;
    glTranslatef(camX * 0.02f, -2.5f + camY * 0.02f, zDistance);
    glRotatef(28.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotationAngle, 0.0f, 1.0f, 0.0f);

    int gridDimM = 6;
    int gridDimN = 7;
    float cellSize = 2.2f;

    float originX = -(gridDimM * cellSize) / 2.0f;
    float originZ = -(gridDimN * cellSize) / 2.0f;

    const auto& stack = engine.getStack();

    // Map active call stack nodes to dynamic surface heights
    auto getDynamicHeight = [&](int mVal, int nVal) -> float {
        double baseVal = 0;
        if (mVal == 0) baseVal = nVal + 1;
        else if (mVal == 1) baseVal = nVal + 2;
        else if (mVal == 2) baseVal = 2 * nVal + 3;
        else if (mVal == 3) baseVal = std::pow(2, nVal + 3) - 3;
        else baseVal = 65533;

        // Hyper-logarithmic base height
        float baseH = static_cast<float>(std::log(1.0 + std::log(1.0 + baseVal))) * 2.0f;

        // Add active stack call energy boost
        float activePulse = 0.0f;
        for (const auto& frame : stack) {
            if (frame.m == mVal && frame.n == nVal) {
                activePulse += 1.2f;
            }
        }

        float wave = std::sin(m_rotationAngle * 0.08f + (mVal + nVal) * 0.6f) * 0.3f;
        return baseH + activePulse + wave;
    };

    // 1. Render Filled 3D Solid Surface Quads with height-based color gradient
    glEnable(GL_BLEND);
    glBegin(GL_QUADS);

    for (int m = 0; m < gridDimM - 1; ++m) {
        for (int n = 0; n < gridDimN - 1; ++n) {
            float x1 = originX + m * cellSize;
            float z1 = originZ + n * cellSize;

            float x2 = originX + (m + 1) * cellSize;
            float z2 = originZ + n * cellSize;

            float x3 = originX + (m + 1) * cellSize;
            float z3 = originZ + (n + 1) * cellSize;

            float x4 = originX + m * cellSize;
            float z4 = originZ + (n + 1) * cellSize;

            float y1 = getDynamicHeight(m, n);
            float y2 = getDynamicHeight(m + 1, n);
            float y3 = getDynamicHeight(m + 1, n + 1);
            float y4 = getDynamicHeight(m, n + 1);

            float avgY = (y1 + y2 + y3 + y4) / 4.0f;
            float ratio = std::min(1.0f, avgY / 7.0f);

            // Shading: Deep obsidian blue at base -> Purple & Electric Gold at peaks
            glColor4f(0.02f + ratio * 0.9f, 0.1f + ratio * 0.3f, 0.3f + ratio * 0.6f, 0.65f);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);
            glVertex3f(x4, y4, z4);
        }
    }
    glEnd();

    // 2. Render Glowing 3D Wireframe Grid Lines Overlay
    glLineWidth(1.8f);
    glBegin(GL_LINES);

    for (int m = 0; m < gridDimM; ++m) {
        for (int n = 0; n < gridDimN; ++n) {
            float x1 = originX + m * cellSize;
            float z1 = originZ + n * cellSize;

            float y1 = getDynamicHeight(m, n);

            if (m < gridDimM - 1) {
                float x2 = originX + (m + 1) * cellSize;
                float y2 = getDynamicHeight(m + 1, n);
                glColor4f(0.0f, 0.95f, 1.0f, 0.85f);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z1);
            }

            if (n < gridDimN - 1) {
                float z3 = originZ + (n + 1) * cellSize;
                float y3 = getDynamicHeight(m, n + 1);
                glColor4f(0.6f, 0.3f, 0.9f, 0.85f);
                glVertex3f(x1, y1, z1);
                glVertex3f(x1, y3, z3);
            }
        }
    }
    glEnd();

    // 3. Render 3D Vertical Laser Beams for Active Stack Nodes
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    for (const auto& frame : stack) {
        if (frame.m < gridDimM && frame.n < gridDimN) {
            float px = originX + frame.m * cellSize;
            float pz = originZ + frame.n * cellSize;
            float py = getDynamicHeight(frame.m, frame.n);

            glColor4f(1.0f, 0.84f, 0.0f, 0.9f);
            glVertex3f(px, py, pz);
            glVertex3f(px, py + 4.5f, pz); // Laser beam up into 3D space
        }
    }
    glEnd();

    // 4. Render Active Stack Pulsing Gold Orbs
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (const auto& frame : stack) {
        if (frame.m < gridDimM && frame.n < gridDimN) {
            float px = originX + frame.m * cellSize;
            float pz = originZ + frame.n * cellSize;
            float py = getDynamicHeight(frame.m, frame.n) + 4.5f;

            glColor4f(1.0f, 0.9f, 0.2f, 1.0f);
            glVertex3f(px, py, pz);
        }
    }
    glEnd();

    // 5. Draw 3D Base Floor Grid
    glColor4f(0.12f, 0.22f, 0.4f, 0.3f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int i = -10; i <= 10; ++i) {
        glVertex3f(static_cast<float>(i), 0.0f, -10.0f);
        glVertex3f(static_cast<float>(i), 0.0f, 10.0f);

        glVertex3f(-10.0f, 0.0f, static_cast<float>(i));
        glVertex3f(10.0f, 0.0f, static_cast<float>(i));
    }
    glEnd();
}

}
