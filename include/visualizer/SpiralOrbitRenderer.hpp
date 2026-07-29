#ifndef SPIRAL_ORBIT_RENDERER_HPP
#define SPIRAL_ORBIT_RENDERER_HPP

#include "AckermannStackEngine.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>

namespace Engine {

struct StepOrbitPoint {
    long long stepIndex;
    int nodeId;
    int m;
    int n;
    int depth;
    bool isResolved;
};

class SpiralOrbitRenderer {
public:
    SpiralOrbitRenderer();

    void reset();
    void render(const AckermannStackEngine& engine, int screenWidth, int screenHeight, float camX, float camY, float camZoom);

private:
    float m_rotationAngle;
    std::vector<StepOrbitPoint> m_historyPoints;
    long long m_lastStepCount;
    int m_lastRootId;
};

}

#endif
