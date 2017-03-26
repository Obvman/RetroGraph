#include "../headers/FontManager.h"

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <GL/glew.h>

#include "../headers/utils.h"
#include "../headers/drawUtils.h"

namespace rg {

FontManager::FontManager() :
    m_hWnd{ nullptr }
{

}

FontManager::~FontManager() {
}

void FontManager::init(HWND hWnd, uint32_t windowHeight) {
    m_hWnd = hWnd;

    /* List of fonts for quick experimentation */
    const char* const typefaces[] = {
        "Courier New",
        "Lato Lights",
        "Orator Std",
        "Verdana",
        "Letter Gothic Std",
        "Kozuka Gothic Pr6N L",
        "Algerian",
        "United Sans Rg Md",
    };

    const auto standardFontHeight{ std::lround(windowHeight / 70.0f) };

    createFont(standardFontHeight, FW_DONTCARE, typefaces[0], RG_FONT_STANDARD);
    createFont(standardFontHeight, FW_BOLD, typefaces[0], RG_FONT_STANDARD_BOLD);
    createFont(72, FW_NORMAL, typefaces[7], RG_FONT_TIME);
    createFont(7*standardFontHeight/8, FW_NORMAL, typefaces[1], RG_FONT_SMALL);
    createFont(3*standardFontHeight/2, FW_BOLD, typefaces[0], RG_FONT_MUSIC_LARGE);
    createFont(standardFontHeight, FW_BOLD, typefaces[7], RG_FONT_MUSIC);

    // Set default font
    glListBase(m_fontBases[RG_FONT_STANDARD]);
}

void FontManager::release() {
    for (const auto base : m_fontBases) {
        glDeleteLists(base, RG_NUM_CHARS_IN_FONT);
    }
}

void FontManager::renderLine(GLfloat rasterX, GLfloat rasterY,
                             RGFONTCODE fontCode, const char* text) const {
    renderLine(rasterX, rasterY, fontCode, text, strlen(text));
}

void FontManager::renderLine(GLfloat rasterX, GLfloat rasterY,
                             RGFONTCODE fontCode, const char* text,
                             size_t textLen) const {
    glRasterPos2f(rasterX, rasterY);

    glPushAttrib(GL_LIST_BIT);
    glListBase(m_fontBases[fontCode]);

    glCallLists(textLen, GL_UNSIGNED_BYTE, text);

    glPopAttrib();
}

void FontManager::renderLine(RGFONTCODE fontCode,
                             const char* text,
                             uint32_t areaX,
                             uint32_t areaY,
                             uint32_t areaWidth,
                             uint32_t areaHeight,
                             int32_t alignFlags,
                             uint32_t alignMarginX,
                             uint32_t alignMarginY) const {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    // Set the area to viewport if default values are given
    if (areaWidth == 0 && areaHeight == 0 && areaX == 0 && areaY == 0) {
        areaWidth = vp[2];
        areaHeight = vp[3];
    } else {
        glViewport(vp[0] + areaX, vp[1] + areaY, areaWidth, areaHeight);
    }

    auto rasterY = float{ 0.0f };
    const auto fontHeightPx{ m_fontCharHeights[fontCode] };
    const auto textLen{ strlen(text) };

    // Handle vertical alignment
    if (alignFlags & RG_ALIGN_CENTERED_VERTICAL) {
        const auto drawYMidPx{ (areaHeight - fontHeightPx) / 2 };
        rasterY = pixelsToVPCoords(drawYMidPx + m_fontCharDescents[fontCode], 
                                   areaHeight);
    } else if (alignFlags & RG_ALIGN_BOTTOM) {
        rasterY = pixelsToVPCoords(alignMarginY + m_fontCharDescents[fontCode],
                                   areaHeight);
    } else if (alignFlags & RG_ALIGN_TOP) {
        rasterY = pixelsToVPCoords(areaHeight - m_fontCharAscents[fontCode] - alignMarginY,
                                   areaHeight);
    }

    auto strWidthPx { calculateStringWidth(text, textLen, fontCode) };

    // If the string is too large, then truncate it and add ellipses
    if (strWidthPx > areaWidth - alignMarginX) {
        char newText[256];

        // Copy char by char into the new buffer while there is enough width
        auto newStrWidthPx = uint32_t{ 0U };
        for (auto i = size_t{ 0U }; i < textLen; ++i) {
            newText[i] = text[i];
            newStrWidthPx += m_fontCharWidths[fontCode][newText[i]];

            // If we've gone over, remove last character and replace chars before
            // with ellipses
            if (newStrWidthPx > areaWidth - alignMarginX) {
                newText[i] = '\0';
                newText[i-1] = '.';
                newText[i-2] = '.';
                break;
            }
        }

        const auto truncTextLen{ strlen(newText) };
        strWidthPx = calculateStringWidth(newText, truncTextLen, fontCode);

        const auto rasterX{ getRasterXAlignment(alignFlags, strWidthPx, 
                areaWidth, alignMarginX) };

        glRasterPos2f(rasterX, rasterY);
        // Render in the specified font, preserving the previously selected font
        glPushAttrib(GL_LIST_BIT); {
            glListBase(m_fontBases[fontCode]);
            glCallLists(truncTextLen, GL_UNSIGNED_BYTE, newText);
        } glPopAttrib();

    } else {
        // Handle horizontal alignment
        const auto rasterX{ getRasterXAlignment(alignFlags, strWidthPx, 
                areaWidth, alignMarginX) };

        glRasterPos2f(rasterX, rasterY);
        glPushAttrib(GL_LIST_BIT); {
            glListBase(m_fontBases[fontCode]);
            glCallLists(textLen, GL_UNSIGNED_BYTE, text);
        } glPopAttrib();

    }
    glViewport(vp[0], vp[1], vp[2], vp[3]);

}

void FontManager::renderLines(RGFONTCODE fontCode,
                              const std::vector<std::string>& lines,
                              uint32_t areaX,
                              uint32_t areaY,
                              uint32_t areaWidth,
                              uint32_t areaHeight,
                              int32_t alignFlags,
                              uint32_t alignMarginX,
                              uint32_t alignMarginY) const {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    /* If width and height are given default values, use the current viewport as area*/
    if (areaWidth == 0 && areaHeight == 0 && areaX == 0 && areaY == 0) {
        areaWidth = vp[2];
        areaHeight = vp[3];
    } else {
        glViewport(vp[0] + areaX, vp[1] + areaY, areaWidth, areaHeight);
    }

    // Set the Y position of the first line according to alignment rules
    const auto renderHeight{ areaHeight - alignMarginY * 2 };
    const auto fontHeight{ m_fontCharHeights[fontCode] };
    const auto rasterLineDeltaY{ (renderHeight-fontHeight)/(lines.size()-1) };

    // Start at top, render downwards
    auto rasterYPx = uint32_t{ areaHeight - alignMarginY - fontHeight };
    if (alignFlags & RG_ALIGN_CENTERED_VERTICAL) {
        //rasterY = float{ 1.0f -
            //1.0f*static_cast<float>(deltaFromTop + descent + 
            //m_fontCharHeights[fontCode])/areaHeight };
    } else if (alignFlags & RG_ALIGN_TOP) {
        /*rasterY = float{ 1.0f -
            static_cast<float>(alignMargin - descent)/areaHeight };*/
    } else if (alignFlags & RG_ALIGN_BOTTOM) {
        // TODO
    }

    for (const auto& str : lines) {
        // Handle X alignment for the string
        const auto strWidthPx{ calculateStringWidth(str.c_str(), str.size(),
                                                    fontCode) };

        const auto rasterX{ getRasterXAlignment(alignFlags, strWidthPx, 
                areaWidth, alignMarginX) };
        const auto rasterY{ pixelsToVPCoords(rasterYPx, areaHeight) };

        // Draw the string
        glRasterPos2f(rasterX, rasterY);
        glPushAttrib(GL_LIST_BIT); {
            glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());
        } glPopAttrib();

        // Set the raster position to the next line
        rasterYPx -= rasterLineDeltaY;
    }

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void FontManager::createFont(uint32_t fontHeight, int32_t weight,
                             const char* typeface, RGFONTCODE code) {
    const auto hdc{ GetDC(m_hWnd) };

    m_fontBases[code] = glGenLists(RG_NUM_CHARS_IN_FONT);
    HFONT hFont = CreateFontA(
        fontHeight, 0, 0, 0, weight,
        FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, typeface);
    SelectObject(hdc, hFont);
    wglUseFontBitmaps(hdc, 0, RG_NUM_CHARS_IN_FONT, m_fontBases[code]);

    setFontCharacteristics(code, hdc);

    DeleteObject(hFont);
}

void FontManager::setFontCharacteristics(RGFONTCODE c, HDC hdc) {
    // Fill character width array
    GetCharWidth32(hdc, 0, RG_NUM_CHARS_IN_FONT-1, &(m_fontCharWidths[c][0]));

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    m_fontCharHeights[c] = tm.tmHeight;
    m_fontCharAscents[c] = tm.tmAscent;
    m_fontCharDescents[c] = tm.tmDescent;
    m_fontCharInternalLeadings[c] = tm.tmInternalLeading;
}

uint32_t FontManager::calculateStringWidth(const char* text, size_t textLen, 
                                          RGFONTCODE c) const {
    auto strWidthPx = int32_t{ 0 };
    for (auto i = size_t{ 0U }; i < textLen; ++i) {
        // Make sure the character is in range, if not, add default value
        if (text[i] > RG_NUM_CHARS_IN_FONT || text[i] < 0) {
            strWidthPx += m_fontCharWidths[c]['A'];
        } else {
            strWidthPx += m_fontCharWidths[c][text[i]];
        }
    }
    return strWidthPx;
}

float FontManager::getRasterXAlignment(int32_t alignFlags, uint32_t strWidthPx, 
                                       uint32_t areaWidth, uint32_t alignMargin) const {
    if (alignFlags & RG_ALIGN_CENTERED_HORIZONTAL) {
        const auto drawXPx{ (areaWidth - strWidthPx) / 2};
        return pixelsToVPCoords(drawXPx, areaWidth);
    } else if (alignFlags & RG_ALIGN_LEFT) {
        return pixelsToVPCoords(alignMargin, areaWidth);
    } else if (alignFlags & RG_ALIGN_RIGHT) {
        return pixelsToVPCoords(areaWidth - strWidthPx - alignMargin, areaWidth);
    }
    return 0.0f;
}


}
