#ifndef QUANTUM_TREE_RENDERER_HPP
#define QUANTUM_TREE_RENDERER_HPP

#include "AckermannStackEngine.hpp"
#include "FontRenderer.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <string>

namespace Engine {

struct Particle {
    float x, y;
    float vx, vy;
    float alpha;
    float radius;
    float r, g, b;
};

struct EdgeParticle {
    float x1, y1, x2, y2;
    float progress;
    float speed;
    float r, g, b;
};

class QuantumTreeRenderer {
public:
    QuantumTreeRenderer();

    void updateLayout(AckermannStackEngine& engine);
    void render(const AckermannStackEngine& engine, int screenWidth, int screenHeight, float camX, float camY, float camZoom);
    void spawnStarburst(float x, float y, float r = 1.0f, float g = 0.84f, float b = 0.0f);
    void spawnEdgeFlow(float x1, float y1, float x2, float y2, float r = 0.0f, float g = 0.95f, float b = 1.0f);

private:
    void drawCircle(float cx, float cy, float r, int numSegments, float red, float green, float blue, float alpha);
    void drawBezierEdge(float x1, float y1, float x2, float y2, float red, float green, float blue);

    std::vector<Particle> m_particles;
    std::vector<EdgeParticle> m_edgeParticles;
};

}

#endif
