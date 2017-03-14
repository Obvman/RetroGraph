#include "../headers/FontManager.h"

#include <iostream>
#include <cmath>

namespace rg {

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


    //RECT r;
    //GetWindowRect(m_hWnd, &r);
    //const auto windowWidth{ r.right - r.left};
    //const auto windowHeight{ r.bottom - r.top};
    const auto standardFontHeight{ std::lround(windowHeight / 70.0f) };
    const auto hdc{ GetDC(m_hWnd) };

    createFont(standardFontHeight, false, typefaces[0], RG_FONT_STANDARD);
    createFont(standardFontHeight, true, typefaces[0], RG_FONT_STANDARD_BOLD);
    createFont(standardFontHeight*6, false, typefaces[1], RG_FONT_TIME);
    createFont(7*standardFontHeight/8, false, typefaces[1], RG_FONT_SMALL);

    // Set default font
    glListBase(m_fontBases[RG_FONT_STANDARD]);
}

void FontManager::release() {
    for (const auto base : m_fontBases) {
        glDeleteLists(base, 256);
    }
}

void FontManager::renderText(GLfloat rasterX, GLfloat rasterY,
                             RGFONTCODE fontCode, const char* text) const {
    renderText(rasterX, rasterY, fontCode, text, strlen(text));
}

void FontManager::renderText(GLfloat rasterX, GLfloat rasterY,
                             RGFONTCODE fontCode, const char* text,
                             size_t textLen) const {
    glRasterPos2f(rasterX, rasterY);

    glPushAttrib(GL_LIST_BIT);
    glListBase(m_fontBases[fontCode]);

    glCallLists(textLen, GL_UNSIGNED_BYTE, text);

    glPopAttrib();
}

void FontManager::createFont(uint32_t fontHeight, bool bold,
                             const char* typeface, RGFONTCODE code) {
    const auto hdc{ GetDC(m_hWnd) };

    m_fontBases[code] = glGenLists(256);
    HFONT hFont = CreateFontA(
        fontHeight, 0, 0, 0, (bold ? FW_BOLD : FW_NORMAL),
        FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, typeface);
    SelectObject(hdc, hFont);
    wglUseFontBitmaps(hdc, 0, 256, m_fontBases[code]);

    TEXTMETRICA tm;
    GetTextMetricsA(hdc, &tm);
    GetCharWidth32A(hdc, 0, 0, &m_fontCharWidths[code]);
    m_fontCharHeights[code] = tm.tmHeight;

    DeleteObject(hFont);
}

}