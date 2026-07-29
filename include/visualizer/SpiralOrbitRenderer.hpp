#ifndef SPIRAL_ORBIT_RENDERER_HPP
#define SPIRAL_ORBIT_RENDERER_HPP

#include "AckermannStackEngine.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace Engine {

class SpiralOrbitRenderer {
public:
    SpiralOrbitRenderer();

    void render(const AckermannStackEngine& engine, int screenWidth, int screenHeight);

private:
    float m_rotationAngle;
};

}

#endif
