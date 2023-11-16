module;

export module Rendering.FontManager;

import Utils;
import Rendering.DrawUtils;

import std.core;

import "GLHeaders.h";
import "WindowsHeaders.h";

namespace rg {

export class FontManager;

export using RGFONTCODE = char;

export enum RG_ALIGN {
    RG_ALIGN_LEFT = 1,
    RG_ALIGN_CENTERED_HORIZONTAL = 2,
    RG_ALIGN_RIGHT = 4,
    RG_ALIGN_TOP = 8,
    RG_ALIGN_CENTERED_VERTICAL = 16,
    RG_ALIGN_BOTTOM = 32,
};

export enum RGFontCode : char {
    RG_FONT_STANDARD = 0U,
    RG_FONT_STANDARD_BOLD = 1U,
    RG_FONT_TIME = 2U,
    RG_FONT_SMALL = 3U,
    RG_FONT_MUSIC_LARGE = 4U,
    RG_FONT_MUSIC = 5U,

    RG_NUM_FONTS
};

export constexpr size_t RG_NUM_CHARS_IN_FONT{ 256 };

class FontManager {
public:
    FontManager(HWND hWnd, int windowHeight);
    ~FontManager() noexcept;
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;
    FontManager& operator=(FontManager&&) = delete;

    /* Release current font data and reinitialises to match new window size */
    void refreshFonts(int newWindowHeight);

    /* Gets the height of the given font's characters in pixels */
    int getFontCharHeight(RGFONTCODE c) const { return m_fontCharHeights[c]; }

    /* Draws the given text in the font specified by fontCode at the raster
       coordinates relative to the current viewport */
    void renderLine(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode, const char* text) const ;
    void renderLine(GLfloat rasterX, GLfloat rasterY, RGFONTCODE fontCode, const char* text, int textLen) const ;

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
                    std::string_view text,
                    int areaX, int areaY, int areaWidth, int areaHeight,
                    int alignFlags = RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL,
                    int alignMarginX = 10U,
                    int alignMarginY = 10U) const;

    /* Renders multiple lines. Assumes lines.size() > 1 */
    void renderLines(RGFONTCODE fontCode,
                     const std::vector<std::string>& lines,
                     int areaX, int areaY, int areaWidth, int areaHeight,
                     int alignFlags = RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL,
                     int alignMarginX = 10U,
                     int alignMarginY = 10U) const;

    void renderLines(RGFONTCODE fontCode,
                     const std::vector<std::string_view>& lines,
                     int areaX, int areaY, int areaWidth, int areaHeight,
                     int alignFlags = RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL,
                     int alignMarginX = 10U,
                     int alignMarginY = 10U) const;

private:
    void initFonts(int windowHeight);

    /* Releases font resources */
    void release();

    /* Creates a new font entry into the fontBases list and retrieves
       character width/pixel information */
    void createFont(int fontHeight, int weight, const char* typeface, RGFONTCODE code);
    void setFontCharacteristics(RGFONTCODE c, HDC hdc);
    int calculateStringWidth(const char* text, size_t textLen, RGFONTCODE c) const;
    int calculateStringWidth(std::string_view text, RGFONTCODE c) const;
    float getRasterXAlignment(int alignFlags, int strWidthPx, int areaWidth, int alignMargin) const;

    HWND m_hWnd{ nullptr };
    std::array<GLuint, RG_NUM_FONTS> m_fontBases{ };
    std::array<std::array<int, RG_NUM_CHARS_IN_FONT>, RG_NUM_FONTS> m_fontCharWidths{ };
    std::array<int, RG_NUM_FONTS> m_fontCharHeights{ };
    std::array<int, RG_NUM_FONTS> m_fontCharAscents{ };
    std::array<int, RG_NUM_FONTS> m_fontCharDescents{ };
    std::array<int, RG_NUM_FONTS> m_fontCharInternalLeadings{ };
};

FontManager::FontManager(HWND hWnd, int windowHeight) :
    m_hWnd{ hWnd } {

    initFonts(windowHeight);
}

FontManager::~FontManager() {
    for (const auto base : m_fontBases) {
        glDeleteLists(base, RG_NUM_CHARS_IN_FONT);
    }
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

    // Set the Y position of the first line according to alignment rules
    const auto renderHeight{ areaHeight - alignMarginY * 2 };
    const auto fontHeight{ m_fontCharHeights[fontCode] };
    const auto rasterLineDeltaY{ (renderHeight - fontHeight) / (static_cast<int>(lines.size()) - 1) };

    // Start at top, render downwards
    auto rasterYPx = int{ areaHeight - alignMarginY - fontHeight };
    if (alignFlags & RG_ALIGN_CENTERED_VERTICAL) {
        // Default behaviour
    } else if (alignFlags & RG_ALIGN_TOP) {
        // TODO
    } else if (alignFlags & RG_ALIGN_BOTTOM) {
        // TODO
    }

    for (const auto& str : lines) {
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

void FontManager::renderLines(RGFONTCODE fontCode,
                              const std::vector<std::string_view>& lines,
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

    // Set the Y position of the first line according to alignment rules
    const auto renderHeight{ areaHeight - alignMarginY * 2 };
    const auto fontHeight{ m_fontCharHeights[fontCode] };
    const auto rasterLineDeltaY{ (renderHeight - fontHeight) / (static_cast<int>(lines.size()) - 1) };

    // Start at top, render downwards
    auto rasterYPx = int{ areaHeight - alignMarginY - fontHeight };
    if (alignFlags & RG_ALIGN_CENTERED_VERTICAL) {
        // Default behaviour
    } else if (alignFlags & RG_ALIGN_TOP) {
        // TODO
    } else if (alignFlags & RG_ALIGN_BOTTOM) {
        // TODO
    }

    for (const auto str : lines) {
        // Handle X alignment for the string
        const auto strWidthPx{ calculateStringWidth(str, fontCode) };

        const auto rasterX{ getRasterXAlignment(alignFlags, strWidthPx,
                                                areaWidth, alignMarginX) };
        const auto rasterY{ pixelsToVPCoords(rasterYPx, areaHeight) };

        // Draw the string
        glRasterPos2f(rasterX, rasterY);
        glListBase(m_fontBases[fontCode]);
        glCallLists(static_cast<GLsizei>(str.size()), GL_UNSIGNED_BYTE, str.data());

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

    createFont(standardFontHeight, FW_BOLD, typefaces[0],
               RG_FONT_STANDARD_BOLD);

    createFont(72, FW_NORMAL, typefaces[7], RG_FONT_TIME);

    createFont(7*standardFontHeight/8, FW_NORMAL, typefaces[1], RG_FONT_SMALL);

    createFont(3*standardFontHeight/2, FW_BOLD, typefaces[0],
               RG_FONT_MUSIC_LARGE);

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
    auto strWidthPx = int{ 0 };
    for (auto i = size_t{ 0U }; i < text.size(); ++i) {
        // Make sure the character is in range, if not, add default value
        if (text[i] > RG_NUM_CHARS_IN_FONT || text[i] < 0) {
            strWidthPx += m_fontCharWidths[c]['A'];
        } else {
            strWidthPx += m_fontCharWidths[c][text[i]];
        }
    }
    return strWidthPx;
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


}
