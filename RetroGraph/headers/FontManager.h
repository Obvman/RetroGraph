#pragma once

#include <Windows.h>
#include <stdint.h>
#include <array>
#include <vector>
#include <string>

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

constexpr size_t RG_NUM_CHARS_IN_FONT{ 256 };

constexpr size_t RG_FONT_STANDARD{ 0U };
constexpr size_t RG_FONT_STANDARD_BOLD{ 1U };
constexpr size_t RG_FONT_TIME{ 2U };
constexpr size_t RG_FONT_SMALL{ 3U };
constexpr size_t RG_FONT_MUSIC_LARGE{ 4U };
constexpr size_t RG_FONT_MUSIC{ 5U };

constexpr size_t RG_NUM_FONTS{ 6U };

class FontManager {
public:
    FontManager();
    ~FontManager();
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    void init(HWND hWnd, uint32_t windowHeight);

    /* Releases font resources */
    void release();

    /* Gets the width of the given font's characters in pixels */
    //int32_t getFontCharWidth(RGFONTCODE c) const { return m_fontCharWidths[c]; }
    /* Gets the height of the given font's characters in pixels */
    int32_t getFontCharHeight(RGFONTCODE c) const { return m_fontCharHeights[c]; }

    /* Draws the given text in the font specified by fontCode at the raster
       coordinates relative to the current viewport */
    void renderLine(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text) const ;

    void renderLine(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode,
                    const char* text, size_t textLen) const ;

    /* Renders a line within the given area. 
     *     fontCode specifies the font to draw the line in
     *     text is the raw text to draw
     *     areaX, areaY, areaWidth, areaHeight specifies a viewport for the 
     *         alignment rules for the text. areaX and areaY are relative to the
     *         current glViewport. If all area values are 0, the current 
     *         glViewport is used.
     *     alignFlags specify the alignment rules. If two conflicting flags are
     *         provided (e.g. two different vertical alignment options), only one
     *         alignment rule will be followed
     *     marginX and marginY are optional values that specify how far from
     *         the edges of the area to draw.
     */
    void renderLine(RGFONTCODE fontCode,
                    const char* text,
                    uint32_t areaX,
                    uint32_t areaY,
                    uint32_t areaWidth,
                    uint32_t areaHeight,
                    int32_t alignFlags = RG_ALIGN_CENTERED_HORIZONTAL |
                                         RG_ALIGN_CENTERED_VERTICAL,
                    uint32_t alignMarginX = 10U,
                    uint32_t alignMarginY = 10U) const;
    /* Renders multiple lines. Requires lines.size() > 1 */
    void renderLines(RGFONTCODE fontCode,
                     const std::vector<std::string>& lines,
                     uint32_t areaX,
                     uint32_t areaY,
                     uint32_t areaWidth,
                     uint32_t areaHeight,
                     int32_t alignFlags = RG_ALIGN_CENTERED_HORIZONTAL |
                                          RG_ALIGN_CENTERED_VERTICAL,
                     uint32_t alignMarginX = 10U,
                     uint32_t alignMarginY = 10U) const;
private:
    /* Creates a new font entry into the fontBases list and retrieves
       character width/pixel information */
    void createFont(uint32_t fontHeight, int32_t weight, const char* typeface,
                    RGFONTCODE code);
    void setFontCharacteristics(RGFONTCODE c, HDC hdc);
    uint32_t calculateStringWidth(const char* text, size_t textLen,
                                 RGFONTCODE c) const;
    float getRasterXAlignment(int32_t alignFlags, uint32_t strWidthPx, 
                              uint32_t areaWidth, uint32_t alignMargin) const;

    HWND m_hWnd;
    std::array<GLuint, RG_NUM_FONTS> m_fontBases;
    std::array<std::array<int32_t, RG_NUM_CHARS_IN_FONT>, RG_NUM_FONTS> m_fontCharWidths;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharHeights;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharAscents;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharDescents;
    std::array<int32_t, RG_NUM_FONTS> m_fontCharInternalLeadings;

    std::array<int32_t, RG_NUM_CHARS_IN_FONT> m_timeFontCharWidths;
};

}
