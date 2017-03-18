#pragma once

#include <stdint.h>
#include <array>

// Forward declarations to avoid including Windows.h
#ifndef _WINDEF_
struct HWND__;
typedef HWND__* HWND;
#endif

#ifndef __GL_H__
typedef unsigned int GLuint;
typedef float GLfloat;
#endif

namespace rg {

using RGFONTCODE = size_t;

enum RG_ALIGN {
    RG_ALIGN_LEFT = 1,
    RG_ALIGN_CENTERED_HORIZONTAL = 2,
    RG_ALIGN_RIGHT = 4,
    RG_ALIGN_TOP = 8,
    RG_ALIGN_CENTERED_VERTICAL = 16,
    RG_ALIGN_BOTTOM = 32,
};

constexpr size_t RG_FONT_STANDARD{ 0U };
constexpr size_t RG_FONT_STANDARD_BOLD{ 1U };
constexpr size_t RG_FONT_TIME{ 2U };
constexpr size_t RG_FONT_SMALL{ 3U };

constexpr size_t RG_NUM_FONTS{ 4U };

class FontManager {
public:
    FontManager();
    ~FontManager();
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    void init(HWND hWnd, uint32_t windowHeight);
    void release();

    /* Gets the width of the given font's characters in pixels */
    int32_t getFontCharWidth(RGFONTCODE c) const { return m_fontCharWidths[c]; }
    /* Gets the height of the given font's characters in pixels */
    int32_t getFontCharHeight(RGFONTCODE c) const { return m_fontCharHeights[c]; }

    /* Draws the given text in the font specified by fontCode at the raster
       coordinates relative to the current viewport */
    void renderLine(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text) const ;
    void renderLine(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text, size_t textLen) const ;
    /* TODO: document fully. If given an area (in pixels, relative to current viewport), aligns the
       text horizontally and vertically inside the box */
    void renderLine(RGFONTCODE fontCode,
                    const char* text,
                    uint32_t areaX,
                    uint32_t areaY,
                    uint32_t areaWidth,
                    uint32_t areaHeight,
                    int32_t alignFlags = RG_ALIGN_CENTERED_HORIZONTAL |
                                         RG_ALIGN_CENTERED_VERTICAL,
                    uint32_t alignMargin = 10) const;
private:
    /* Creates a new font entry into the fontBases list and retrieves
       character width/pixel information */
    void createFont(uint32_t fontHeight, bool bold, const char* typeface,
                    RGFONTCODE code);

    HWND m_hWnd;
    std::array<GLuint, RG_NUM_FONTS> m_fontBases;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharWidths;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharHeights;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharAscents;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharDescents;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharInternalLeadings;
};

}