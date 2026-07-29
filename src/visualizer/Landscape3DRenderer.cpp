#include "../../include/visualizer/Landscape3DRenderer.hpp"
#include <cmath>
#include <algorithm>

namespace Engine {

Landscape3DRenderer::Landscape3DRenderer() : m_rotationAngle(0.0f), m_initialized(false) {}

void Landscape3DRenderer::init() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    m_initialized = true;
}

void Landscape3DRenderer::render(const AckermannStackEngine& engine, int screenWidth, int screenHeight) {
    (void)engine;
    if (!m_initialized) init();

    glViewport(0, 0, screenWidth, screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // Simple 3D perspective projection
    float fov = 60.0f;
    float zNear = 0.1f;
    float zFar = 1000.0f;
    float fH = tanf(fov / 360.0f * 3.1415926f) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_rotationAngle += 0.4f;

    // Orbital camera
    glTranslatef(0.0f, -2.5f, -18.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotationAngle, 0.0f, 1.0f, 0.0f);

    int gridDimM = 5;
    int gridDimN = 6;
    float cellSize = 2.0f;

    float originX = -(gridDimM * cellSize) / 2.0f;
    float originZ = -(gridDimN * cellSize) / 2.0f;

    // Render 3D heightmap wireframe grid
    glLineWidth(1.5f);
    glBegin(GL_LINES);

    for (int m = 0; m < gridDimM; ++m) {
        for (int n = 0; n < gridDimN; ++n) {
            float x1 = originX + m * cellSize;
            float z1 = originZ + n * cellSize;

            float x2 = originX + (m + 1) * cellSize;
            float z2 = originZ + n * cellSize;

            float x3 = originX + m * cellSize;
            float z3 = originZ + (n + 1) * cellSize;

            // Height mapping formula Z = ln(1 + ln(1 + A(m, n)))
            auto calcH = [](int mVal, int nVal) -> float {
                double val = 0;
                if (mVal == 0) val = nVal + 1;
                else if (mVal == 1) val = nVal + 2;
                else if (mVal == 2) val = 2 * nVal + 3;
                else if (mVal == 3) val = std::pow(2, nVal + 3) - 3;
                else val = 65533;
                return static_cast<float>(std::log(1.0 + std::log(1.0 + val))) * 1.8f;
            };

            float y1 = calcH(m, n);
            float y2 = calcH(m + 1, n);
            float y3 = calcH(m, n + 1);

            // Neon line color ramp
            glColor4f(0.0f, 0.95f, 1.0f, 0.8f);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);

            glColor4f(0.6f, 0.3f, 0.9f, 0.8f);
            glVertex3f(x1, y1, z1);
            glVertex3f(x3, y3, z3);
        }
    }
    glEnd();

    // Draw base floor grid
    glColor4f(0.2f, 0.3f, 0.5f, 0.3f);
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
