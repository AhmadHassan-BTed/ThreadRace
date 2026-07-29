#ifndef LANDSCAPE_3D_RENDERER_HPP
#define LANDSCAPE_3D_RENDERER_HPP

#include "AckermannStackEngine.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>

namespace Engine {

class Landscape3DRenderer {
public:
    Landscape3DRenderer();

    void init();
    void render(const AckermannStackEngine& engine, int screenWidth, int screenHeight);

private:
    float m_rotationAngle;
    bool m_initialized;
};

}

#endif
