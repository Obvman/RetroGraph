export module Rendering.GLListContainer;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export constexpr auto serifLen = float{ 0.05f };

export class GLListContainer {
public:
    static GLListContainer& inst() { static GLListContainer i; return i; }

    ~GLListContainer();

    void drawViewportBorder() const { glCallList(m_vpBorderList); }
    void drawBorder() const { glCallList(m_borderList); }
    void drawWidgetBackground() const { glCallList(m_widgetBGList); }
    void drawTopAndBottomSerifs() const { glCallList(m_serifList); }

private:
    GLListContainer();

    void initViewportBorderList() const;
    void initBorderList() const;
    void initSerifList() const;
    void initWidgetBGList() const;

    GLuint m_vpBorderList;
    GLuint m_borderList;
    GLuint m_serifList;
    GLuint m_widgetBGList;
};

} // namespace rg
