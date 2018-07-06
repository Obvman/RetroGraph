#pragma once

#include "stdafx.h"
// #include <GL/GL.h>
// #include <GL/glew.h>

#include "drawUtils.h"

namespace rg {

class ListContainer {
public:
    static ListContainer& inst() { static ListContainer i; return i; }

    ~ListContainer();

    void drawCircle() const { glCallList(m_circleList); }
    void drawViewportBorder() const { glCallList(m_vpBorderList); }
    void drawBorder() const { glCallList(m_borderList); }
    void drawWidgetBackground() const { glCallList(m_widgetBGList); }
    void drawTopAndBottomSerifs() const { glCallList(m_serifList); }
private:
    ListContainer();

    void initCircleList() const;
    void initViewportBorderList() const;
    void initBorderList() const;
    void initSerifList() const;
    void initWidgetBGList() const;

    GLuint m_listStart;

    GLuint m_vpBorderList;
    GLuint m_borderList;
    GLuint m_serifList;
    GLuint m_circleList;
    GLuint m_widgetBGList;
};

}
