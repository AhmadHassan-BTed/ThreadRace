#ifndef RACE_ARENA_RENDERER_HPP
#define RACE_ARENA_RENDERER_HPP

#include "RaceEngine.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace Engine {

class RaceArenaRenderer {
public:
    RaceArenaRenderer();

    void render(const RaceEngine& raceEngine, int screenWidth, int screenHeight);

private:
    void drawProgressBar(float x, float y, float w, float h, float percent, float r, float g, float b, const std::string& label);
    void drawSpeedometerGauge(float cx, float cy, float radius, float valPercent, float r, float g, float b, const std::string& title, const std::string& valueStr);
    void drawFinishLine(float x, float y, float w, float h);

    float m_animTime;
};

}

#endif
