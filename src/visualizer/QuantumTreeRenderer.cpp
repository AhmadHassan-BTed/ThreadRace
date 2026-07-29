#include "../../include/visualizer/QuantumTreeRenderer.hpp"
#include "../../include/visualizer/FontRenderer.hpp"
#include <cmath>
#include <algorithm>

namespace Engine {

QuantumTreeRenderer::QuantumTreeRenderer() {}

void QuantumTreeRenderer::spawnStarburst(float x, float y, float r, float g, float b) {
    for (int i = 0; i < 16; ++i) {
        float angle = (2.0f * M_PI * i) / 16.0f;
        float speed = 1.5f + static_cast<float>(rand() % 100) / 40.0f;
        Particle p;
        p.x = x;
        p.y = y;
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        p.alpha = 1.0f;
        p.radius = 3.0f + static_cast<float>(rand() % 10) / 5.0f;
        p.r = r; p.g = g; p.b = b;
        m_particles.push_back(p);
    }
}

void QuantumTreeRenderer::spawnEdgeFlow(float x1, float y1, float x2, float y2, float r, float g, float b) {
    EdgeParticle ep;
    ep.x1 = x1; ep.y1 = y1;
    ep.x2 = x2; ep.y2 = y2;
    ep.progress = 0.0f;
    ep.speed = 0.05f + static_cast<float>(rand() % 50) / 1000.0f;
    ep.r = r; ep.g = g; ep.b = b;
    m_edgeParticles.push_back(ep);
}

void QuantumTreeRenderer::updateLayout(AckermannStackEngine& engine) {
    auto& nodes = const_cast<std::map<int, AckermannNode>&>(engine.getNodes());
    if (nodes.empty()) return;

    int rootId = engine.getRootNodeId();
    if (rootId == -1) return;

    std::function<float(int)> computeSubtreeWidth = [&](int nodeId) -> float {
        auto it = nodes.find(nodeId);
        if (it == nodes.end() || it->second.children.empty()) {
            if (it != nodes.end()) it->second.subWidth = 60.0f;
            return 60.0f;
        }
        float width = 0.0f;
        for (int childId : it->second.children) {
            width += computeSubtreeWidth(childId);
        }
        it->second.subWidth = std::max(width, 60.0f);
        return it->second.subWidth;
    };

    computeSubtreeWidth(rootId);

    std::function<void(int, float, int)> assignCoords = [&](int nodeId, float startX, int depth) {
        auto it = nodes.find(nodeId);
        if (it == nodes.end()) return;

        AckermannNode& node = it->second;
        node.targetX = startX + node.subWidth / 2.0f;
        node.targetY = depth * 85.0f + 60.0f;

        if (node.x == 0.0f && node.y == 0.0f) {
            node.x = node.targetX;
            node.y = node.targetY;
        } else {
            node.x += (node.targetX - node.x) * 0.25f;
            node.y += (node.targetY - node.y) * 0.25f;
        }

        float currentX = startX;
        for (int childId : node.children) {
            auto childIt = nodes.find(childId);
            if (childIt != nodes.end()) {
                assignCoords(childId, currentX, depth + 1);
                currentX += childIt->second.subWidth;
            }
        }
    };

    assignCoords(rootId, -nodes[rootId].subWidth / 2.0f, 0);

    // Spawn edge energy flows along active stack edges
    const auto& stack = engine.getStack();
    if (stack.size() >= 2) {
        int childNodeId = stack.back().nodeId;
        int parentNodeId = stack[stack.size() - 2].nodeId;
        auto pIt = nodes.find(parentNodeId);
        auto cIt = nodes.find(childNodeId);
        if (pIt != nodes.end() && cIt != nodes.end()) {
            spawnEdgeFlow(pIt->second.x, pIt->second.y, cIt->second.x, cIt->second.y, 0.0f, 0.95f, 1.0f);
        }
    }
}

void QuantumTreeRenderer::drawCircle(float cx, float cy, float r, int numSegments, float red, float green, float blue, float alpha) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(red, green, blue, alpha);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(numSegments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void QuantumTreeRenderer::drawBezierEdge(float x1, float y1, float x2, float y2, float red, float green, float blue) {
    glColor4f(red, green, blue, 0.65f);
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
    int numSegments = 20;
    float cpY1 = y1 + (y2 - y1) * 0.5f;
    float cpY2 = y1 + (y2 - y1) * 0.5f;

    for (int i = 0; i <= numSegments; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(numSegments);
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        float x = uuu * x1 + 3 * uu * t * x1 + 3 * u * tt * x2 + ttt * x2;
        float y = uuu * y1 + 3 * uu * t * cpY1 + 3 * u * tt * cpY2 + ttt * y2;
        glVertex2f(x, y);
    }
    glEnd();
}

void QuantumTreeRenderer::render(const AckermannStackEngine& engine, int screenWidth, int screenHeight, float camX, float camY, float camZoom) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-screenWidth / 2.0, screenWidth / 2.0, screenHeight / 2.0, -screenHeight / 2.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(camZoom, camZoom, 1.0f);
    glTranslatef(camX, camY, 0.0f);

    const auto& nodes = engine.getNodes();
    const auto& stack = engine.getStack();
    int activeNodeId = stack.empty() ? -1 : stack.back().nodeId;

    // Render Edges
    for (const auto& kv : nodes) {
        const auto& node = kv.second;
        for (int childId : node.children) {
            auto it = nodes.find(childId);
            if (it != nodes.end()) {
                const auto& child = it->second;
                if (child.status == NodeStatus::RESOLVED) {
                    drawBezierEdge(node.x, node.y, child.x, child.y, 0.0f, 1.0f, 0.53f);
                } else {
                    drawBezierEdge(node.x, node.y, child.x, child.y, 0.0f, 0.95f, 1.0f);
                }
            }
        }
    }

    // Render Edge Energy Flow Particles
    for (auto it = m_edgeParticles.begin(); it != m_edgeParticles.end(); ) {
        it->progress += it->speed;
        if (it->progress >= 1.0f) {
            it = m_edgeParticles.erase(it);
        } else {
            float px = it->x1 + (it->x2 - it->x1) * it->progress;
            float py = it->y1 + (it->y2 - it->y1) * it->progress;
            drawCircle(px, py, 4.0f, 12, it->r, it->g, it->b, 0.9f);
            ++it;
        }
    }

    // Render Nodes & Values
    for (const auto& kv : nodes) {
        const auto& node = kv.second;
        bool isLeaf = (node.m == 0);
        bool isResolved = (node.status == NodeStatus::RESOLVED);
        bool isActive = (node.id == activeNodeId);

        float r = 0.0f, g = 0.95f, b = 1.0f; // Cyan
        if (isResolved) { r = 0.0f; g = 1.0f; b = 0.53f; } // Green
        else if (isLeaf) { r = 1.0f; g = 0.84f; b = 0.0f; } // Gold

        float radius = isLeaf ? 16.0f : 20.0f;
        if (isActive) {
            // Active Node Pulsing Energy Target Ring
            drawCircle(node.x, node.y, radius * 2.0f, 28, 1.0f, 0.2f, 0.6f, 0.45f);
        }

        // Outer Glow
        drawCircle(node.x, node.y, radius * 1.35f, 24, r, g, b, 0.25f);
        // Inner Core
        drawCircle(node.x, node.y, radius, 24, r * 0.25f, g * 0.25f, b * 0.25f, 0.95f);
        // Ring
        drawCircle(node.x, node.y, radius * 0.85f, 20, r, g, b, 0.8f);

        // Render clean node text using game-engine FontRenderer
        std::string label = isResolved ? ("=" + std::to_string(node.result)) : (std::to_string(node.m) + "," + std::to_string(node.n));
        float txtX = node.x - (label.length() * 3.5f);
        float txtY = node.y - 6.0f;
        FontRenderer::getInstance().renderText(txtX, txtY, label, 0.65f, 1.0f, 1.0f, 1.0f, 0.98f);
    }

    // Update & Render Starburst Particles
    for (auto it = m_particles.begin(); it != m_particles.end(); ) {
        it->x += it->vx;
        it->y += it->vy;
        it->alpha -= 0.02f;

        if (it->alpha <= 0.0f) {
            it = m_particles.erase(it);
        } else {
            drawCircle(it->x, it->y, it->radius, 12, it->r, it->g, it->b, it->alpha);
            ++it;
        }
    }
}

}
