export module Rendering.FontManager;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

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

export class FontManager {
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

} // namespace rg
