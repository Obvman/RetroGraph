#pragma once

#include <stdint.h>
#include <vector>
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

    void init(HWND hWnd);
    void release();

    int32_t getFontCharWidth(RGFONTCODE c) const { return m_fontCharWidths[c]; }
    int32_t getFontCharHeight(RGFONTCODE c) const { return m_fontCharHeights[c]; }

    void renderText(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text) const ;
    void renderText(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text, size_t textLen) const ;
private:
    void createFont(uint32_t fontHeight, bool bold, const char* typeface,
                    RGFONTCODE code);

    HWND m_hWnd;

    std::vector<GLuint> m_fontBases;
    std::vector<int32_t> m_fontCharWidths;
    std::vector<int32_t> m_fontCharHeights;
};

}