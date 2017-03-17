#include "../headers/FontManager.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <GL/glew.h>

#include "../headers/utils.h"

namespace rg {

constexpr float pixelsToVPCoords(uint32_t p, uint32_t vpWidth) {
    return (static_cast<float>(p) / vpWidth) * 2.0f - 1.0f;
}

constexpr uint32_t vpCoordsToPixels(float vpCoord, uint32_t vpWidth) {
    return static_cast<uint32_t>(((vpCoord + 1.0f) / 2.0f) * vpWidth);
}

FontManager::FontManager() :
    m_hWnd{ nullptr }
{

}


FontManager::~FontManager() {
}


void FontManager::init(HWND hWnd, uint32_t windowWidth, uint32_t windowHeight) {
    m_hWnd = hWnd;

    /* List of fonts for quick experimentation */
    const char* const typefaces[] = {
        "Courier New",
        "Lato Lights",
        "Orator Std"
        "Verdana",
        "Letter Gothic Std",
        "Kozuka Gothic Pr6N L",
        "Algerian",
    };

    const auto standardFontHeight{ std::lround(windowHeight / 70.0f) };
    const auto hdc{ GetDC(m_hWnd) };

    createFont(standardFontHeight, false, typefaces[0], RG_FONT_STANDARD);
    createFont(standardFontHeight, true, typefaces[0], RG_FONT_STANDARD_BOLD);
    createFont(standardFontHeight*5, false, typefaces[0], RG_FONT_TIME);
    createFont(7*standardFontHeight/8, false, typefaces[1], RG_FONT_SMALL);

    // Set default font
    glListBase(m_fontBases[RG_FONT_STANDARD]);
}

void FontManager::release() {
    for (const auto base : m_fontBases) {
        glDeleteLists(base, 256);
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
                             uint32_t alignMargin) const {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    glViewport(vp[0] + areaX, vp[1] + areaY, areaWidth, areaHeight);

    auto rasterY = float{ 0.0f };
    const auto fontHeightPx{ m_fontCharHeights[fontCode] };
    if (alignFlags & RG_ALIGN_CENTERED_VERTICAL) {
        const auto drawYPx{ (areaHeight - fontHeightPx) / 1 };
        rasterY = pixelsToVPCoords(drawYPx, areaHeight);
    } else if (alignFlags & RG_ALIGN_BOTTOM) {
        rasterY = pixelsToVPCoords(alignMargin + m_fontCharDescents[fontCode], areaHeight);
    } else if (alignFlags & RG_ALIGN_TOP) {
        rasterY = pixelsToVPCoords(areaHeight - m_fontCharAscents[fontCode] - alignMargin,
                                   areaHeight);
    }

    auto rasterX = float{ 0.0f };
    const auto strWidthPx{ strlen(text) * m_fontCharWidths[fontCode] };
    if (alignFlags & RG_ALIGN_CENTERED_HORIZONTAL) {
        const auto drawXPx{ (areaWidth - strWidthPx) / 2};
        rasterX = pixelsToVPCoords(areaX + drawXPx, areaWidth);
    } else if (alignFlags & RG_ALIGN_LEFT) {
        rasterX = pixelsToVPCoords(alignMargin, areaWidth);
    } else if (alignFlags & RG_ALIGN_RIGHT) {
        rasterX = pixelsToVPCoords(areaWidth - strWidthPx, areaWidth);
    }

    if (fontCode == RG_FONT_STANDARD_BOLD) {
        std::cout << "Our calculated width: " << strWidthPx << " for " << text << '\n';
        std::cout << "Our calculated height: " << fontHeightPx << " for " << text << '\n';
    }

    glRasterPos2f(rasterX, rasterY);

    // Render in the specified font, preserving the previously selected font
    glPushAttrib(GL_LIST_BIT); {
        glListBase(m_fontBases[fontCode]);
        glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
    } glPopAttrib();

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void FontManager::createFont(uint32_t fontHeight, bool bold,
                             const char* typeface, RGFONTCODE code) {
    const auto hdc{ GetDC(m_hWnd) };

    m_fontBases[code] = glGenLists(256);
    HFONT hFont = CreateFontA(
        fontHeight, 0, 0, 0, (bold ? FW_BOLD : FW_NORMAL),
        FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, typeface);
    SelectObject(hdc, hFont);
    wglUseFontBitmaps(hdc, 0, 256, m_fontBases[code]);

    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    GetCharWidth32(hdc, 0, 0, &m_fontCharWidths[code]);
    m_fontCharHeights[code] = tm.tmHeight;
    m_fontCharAscents[code] = tm.tmAscent;
    m_fontCharDescents[code] = tm.tmDescent;
    m_fontCharInternalLeadings[code] = tm.tmInternalLeading;

    if (code == RG_FONT_STANDARD) {
        SIZE s;
        GetTextExtentPointA(GetDC(m_hWnd), "Network", strlen("Network"), &s);
        std::cout << "GetTextExtentPoint width: " << s.cx << '\n';
        std::cout << "GetTextExtentPoint height: " << s.cy << '\n';
    }

    DeleteObject(hFont);
}

}