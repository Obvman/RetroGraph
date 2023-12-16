module RG.Rendering:FontManager;

import :DrawUtils;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

FontManager::FontManager(HWND hWnd, int windowHeight)
    : m_hWnd{ hWnd } {

    initFonts(windowHeight);
}

FontManager::~FontManager() {
    release();
}

void FontManager::release() {
    for (const auto base : m_fontBases) {
        glDeleteLists(base, RG_NUM_CHARS_IN_FONT);
    }
}

void FontManager::refreshFonts(int newWindowHeight) {
    release();
    initFonts(newWindowHeight);
}

void FontManager::renderLine(GLfloat rasterX, GLfloat rasterY,
                             RGFONTCODE fontCode, const char* text) const {
    renderLine(rasterX, rasterY, fontCode, text, static_cast<int>(strlen(text)));
}

void FontManager::renderLine(GLfloat rasterX, GLfloat rasterY,
                             RGFONTCODE fontCode, const char* text,
                             int textLen) const {
    glRasterPos2f(rasterX, rasterY);

    glListBase(m_fontBases[fontCode]);
    glCallLists(textLen, GL_UNSIGNED_BYTE, text);
}

void FontManager::renderLine(RGFONTCODE fontCode,
                             std::string_view text,
                             int areaX,
                             int areaY,
                             int areaWidth,
                             int areaHeight,
                             int alignFlags,
                             int alignMarginX /*=10U*/,
                             int alignMarginY /*=10U*/) const {
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
    const auto textLen{ text.size() };

    // Handle vertical alignment
    if (alignFlags & RG_ALIGN_CENTERED_VERTICAL) {
        const auto drawYMidPx{ (areaHeight - fontHeightPx) / 2 };
        rasterY = pixelsToVPCoords(drawYMidPx + m_fontCharDescents[fontCode], 
                                   areaHeight);
    } else if (alignFlags & RG_ALIGN_BOTTOM) {
        rasterY = pixelsToVPCoords(alignMarginY + m_fontCharDescents[fontCode],
                                   areaHeight);
    } else if (alignFlags & RG_ALIGN_TOP) {
        rasterY = pixelsToVPCoords(areaHeight - m_fontCharAscents[fontCode] -
                                   alignMarginY,
                                   areaHeight);
    }

    auto strWidthPx { calculateStringWidth(text, fontCode) };

    // If the string is too large, then truncate it and add ellipses
    if (strWidthPx > areaWidth - alignMarginX) {
        char newText[255];

        // Copy char by char into the new buffer while there is enough width
        auto newStrWidthPx = int{ 0U };
        for (auto i = size_t{ 0U }; i < textLen; ++i) {
            newText[i] = text[i];
            newStrWidthPx += m_fontCharWidths[fontCode][newText[i]];

            // If we've gone over, remove last character and replace chars before
            // with ellipses
            if (newStrWidthPx > areaWidth - alignMarginX) {
                // Ensure there are enough characters to put in ellipses
                if (i > 2) {
                    newText[i] = '\0';
                    newText[i - 1] = '.';
                    newText[i - 2] = '.';
                } else {
                    newText[i] = '\0';
                }
                break;
            }
        }

        const auto truncTextLen{ strlen(newText) };
        strWidthPx = calculateStringWidth(newText, truncTextLen, fontCode);

        const auto rasterX{ getRasterXAlignment(alignFlags, strWidthPx, areaWidth, alignMarginX) };

        glRasterPos2f(rasterX, rasterY);
        // Render in the specified font, preserving the previously selected font
        glPushAttrib(GL_LIST_BIT); {
            glListBase(m_fontBases[fontCode]);
            glCallLists(static_cast<GLsizei>(truncTextLen), GL_UNSIGNED_BYTE, newText);
        } glPopAttrib();

    } else {
        // Handle horizontal alignment
        const auto rasterX{ getRasterXAlignment(alignFlags, strWidthPx, 
                areaWidth, alignMarginX) };

        glRasterPos2f(rasterX, rasterY);
        glListBase(m_fontBases[fontCode]);
        glCallLists(static_cast<GLsizei>(textLen), GL_UNSIGNED_BYTE, text.data());

    }
    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void FontManager::renderLines(RGFONTCODE fontCode,
                              const std::vector<std::string>& lines,
                              int areaX,
                              int areaY,
                              int areaWidth,
                              int areaHeight,
                              int alignFlags,
                              int alignMarginX /*=10U*/,
                              int alignMarginY /*=10U*/) const {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    /* If width and height are given default values, use the current viewport 
       as area */
    if (areaWidth == 0 && areaHeight == 0 && areaX == 0 && areaY == 0) {
        areaWidth = vp[2];
        areaHeight = vp[3];
    } else {
        glViewport(vp[0] + areaX, vp[1] + areaY, areaWidth, areaHeight);
    }

    auto [rasterYPx, rasterLineDeltaY, maxRenderableLines] =
        calculateLinesRenderParameters(static_cast<int>(lines.size()), fontCode, alignFlags, areaHeight, alignMarginY);

    // Start at top, render downwards
    for (int i{ 0 }; i < maxRenderableLines; ++i) {
        const auto& str{ lines[i] };
        // Handle X alignment for the string
        const auto strWidthPx{ calculateStringWidth(str.c_str(), str.size(), fontCode) };

        const auto rasterX{ getRasterXAlignment(alignFlags, strWidthPx,
                                                areaWidth, alignMarginX) };
        const auto rasterY{ pixelsToVPCoords(rasterYPx, areaHeight) };

        // Draw the string
        glRasterPos2f(rasterX, rasterY);
        glListBase(m_fontBases[fontCode]);
        glCallLists(static_cast<GLsizei>(str.size()), GL_UNSIGNED_BYTE, str.c_str());

        // Set the raster position to the next line
        rasterYPx -= static_cast<decltype(rasterYPx)>(rasterLineDeltaY);
    }

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void FontManager::initFonts(int windowHeight) {
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

    createFont(7*standardFontHeight/8, FW_NORMAL, typefaces[0], RG_FONT_SMALL);

    createFont(3*standardFontHeight/2, FW_BOLD, typefaces[0], RG_FONT_MUSIC_LARGE);

    createFont(standardFontHeight, FW_BOLD, typefaces[7], RG_FONT_MUSIC);

    // Set default font
    glListBase(m_fontBases[RG_FONT_STANDARD]);
}

void FontManager::createFont(int fontHeight, int weight,
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
    ReleaseDC(m_hWnd, hdc);

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

int FontManager::calculateStringWidth(const char* text, size_t textLen, RGFONTCODE c) const {
    auto strWidthPx = int{ 0 };
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

int FontManager::calculateStringWidth(std::string_view text, RGFONTCODE c) const {
    return calculateStringWidth(text.data(), text.size(), c);
}

std::tuple<int, int, int> FontManager::calculateLinesRenderParameters(int numLines, RGFONTCODE code, int alignFlags,
                                                                      int areaHeight, int marginY) const {
    const auto renderHeight{ areaHeight - marginY * 2 };
    const auto fontHeight{ m_fontCharHeights[code] };
    const auto fontAscent{ m_fontCharAscents[code] };
    int maxRenderableLines{ std::min(numLines, renderHeight / fontHeight) };

    int rasterYPx;
    int rasterLineDeltaY{ fontHeight + marginY };
    if (alignFlags & RG_ALIGN_CENTERED_VERTICAL) {
        int const freeVerticalSpace{ renderHeight - (fontHeight * maxRenderableLines) };
        int const linePadding{ std::lround(freeVerticalSpace / static_cast<float>(maxRenderableLines + 1)) };
        rasterLineDeltaY = fontHeight + linePadding;
        rasterYPx = areaHeight - marginY - linePadding - fontAscent;
    } else if (alignFlags & RG_ALIGN_BOTTOM) {
        rasterYPx = (maxRenderableLines * (fontHeight + marginY)) - fontAscent;
    } else { // RG_ALIGN_TOP
        rasterYPx = areaHeight - marginY - fontHeight;
    }

    return { rasterYPx, rasterLineDeltaY, maxRenderableLines };
}

float FontManager::getRasterXAlignment(int alignFlags, int strWidthPx,
                                       int areaWidth,
                                       int alignMargin) const {
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

} // namespace rg
