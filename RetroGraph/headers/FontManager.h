#pragma once

#include <stdint.h>
#include <array>
#include <Windows.h>
#include <GL/gl.h>

namespace rg {

using RGFONTCODE = size_t;

constexpr size_t RG_FONT_STANDARD{ 0U };
constexpr size_t RG_FONT_STANDARD_BOLD{ 1U };
constexpr size_t RG_FONT_TIME{ 2U };
constexpr size_t RG_FONT_SMALL{ 3U };

constexpr size_t RG_NUM_FONTS{ 4U };

class FontManager {
public:
    FontManager();
    ~FontManager();

    void init(HWND hWnd, uint32_t windowWidth, uint32_t windowHeight);
    void release();

    /* Gets the width of the given font's characters in pixels */
    int32_t getFontCharWidth(RGFONTCODE c) const { return m_fontCharWidths[c]; }
    /* Gets the height of the given font's characters in pixels */
    int32_t getFontCharHeight(RGFONTCODE c) const { return m_fontCharHeights[c]; }

    /* Draws the given text in the font specified by fontCode at the raster
       coordinates relative to the current viewport */
    void renderText(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text) const ;
    void renderText(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text, size_t textLen) const ;
private:
    /* Creates a new font entry into the fontBases list and retrieves
       character width/pixel information */
    void createFont(uint32_t fontHeight, bool bold, const char* typeface,
                    RGFONTCODE code);

    HWND m_hWnd;
    std::array<GLuint, RG_NUM_FONTS> m_fontBases;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharWidths;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharHeights;
};

}