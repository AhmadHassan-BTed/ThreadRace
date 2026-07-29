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

class QuantumTreeRenderer {
public:
    QuantumTreeRenderer();

    void updateLayout(AckermannStackEngine& engine);
    void render(const AckermannStackEngine& engine, int screenWidth, int screenHeight);
    void spawnStarburst(float x, float y, float r = 1.0f, float g = 0.84f, float b = 0.0f);

    void pan(float dx, float dy);
    void zoom(float factor);
    void resetCamera();

private:
    void drawCircle(float cx, float cy, float r, int numSegments, float red, float green, float blue, float alpha);
    void drawBezierEdge(float x1, float y1, float x2, float y2, float red, float green, float blue);

    float m_camX, m_camY, m_camZoom;
    float m_targetCamX, m_targetCamY, m_targetCamZoom;
    std::vector<Particle> m_particles;
};

}

#endif
