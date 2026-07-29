#include "../../include/visualizer/SpiralOrbitRenderer.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Engine {

SpiralOrbitRenderer::SpiralOrbitRenderer() : m_rotationAngle(0.0f) {}

void SpiralOrbitRenderer::render(const AckermannStackEngine& engine, int screenWidth, int screenHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-screenWidth / 2.0, screenWidth / 2.0, screenHeight / 2.0, -screenHeight / 2.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_rotationAngle += 0.2f;
    glRotatef(m_rotationAngle, 0.0f, 0.0f, 1.0f);

    float phi = 1.61803398875f; // Golden Ratio

    // Render Golden Spiral background guide curve
    glColor4f(1.0f, 0.84f, 0.0f, 0.2f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (float theta = 0; theta < 37.68f; theta += 0.1f) {
        float r = std::pow(phi, theta * 0.25f) * 8.0f;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();

    const auto& nodes = engine.getNodes();
    struct Coords { int id; float x; float y; NodeStatus status; bool isLeaf; };
    std::vector<Coords> coordList;

    int stepIdx = 0;
    for (const auto& kv : nodes) {
        stepIdx++;
        const auto& node = kv.second;
        int depth = node.depth + 1;

        float r = std::sqrt(depth) * std::log(node.m + node.n + 2.0f) * 45.0f + (stepIdx * 1.8f);
        float theta = stepIdx * 2.0f * 3.1415926f * (phi - 1.0f) + (node.m * 3.1415926f / 4.0f);

        float x = r * cosf(theta);
        float y = r * sinf(theta);

        coordList.push_back({ node.id, x, y, node.status, node.m == 0 });
    }

    // Render connecting edge lines
    glColor4f(0.0f, 0.95f, 1.0f, 0.4f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    for (const auto& kv : nodes) {
        const auto& node = kv.second;
        auto pIt = std::find_if(coordList.begin(), coordList.end(), [&](const Coords& c) { return c.id == node.id; });
        if (pIt != coordList.end()) {
            for (int childId : node.children) {
                auto cIt = std::find_if(coordList.begin(), coordList.end(), [&](const Coords& c) { return c.id == childId; });
                if (cIt != coordList.end()) {
                    glVertex2f(pIt->x, pIt->y);
                    glVertex2f(cIt->x, cIt->y);
                }
            }
        }
    }
    glEnd();

    // Render node points
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (const auto& c : coordList) {
        if (c.status == NodeStatus::RESOLVED) glColor4f(0.0f, 1.0f, 0.53f, 0.9f);
        else if (c.isLeaf) glColor4f(1.0f, 0.84f, 0.0f, 0.9f);
        else glColor4f(0.0f, 0.95f, 1.0f, 0.9f);

        glVertex2f(c.x, c.y);
    }
    glEnd();
}

}
