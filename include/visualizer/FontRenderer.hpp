#ifndef FONT_RENDERER_HPP
#define FONT_RENDERER_HPP

#include <SDL2/SDL_opengl.h>
#include <string>
#include <vector>

namespace Engine {

class FontRenderer {
public:
    static FontRenderer& getInstance();

    void init();
    void renderText(float x, float y, const std::string& text, float scale = 1.0f, float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f);
    void renderCardHUD(float x, float y, float width, float height, const std::vector<std::string>& lines);

private:
    FontRenderer();
    ~FontRenderer();

    GLuint m_textureId;
    bool m_initialized;

    void buildFontAtlas();
};

}

#endif
