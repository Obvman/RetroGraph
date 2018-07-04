#pragma once

#include "stdafx.h"
// #include <GL/GL.h>
// #include <GL/glew.h>

#include "drawUtils.h"

namespace rg {

class GLListContainer {
public:
    static GLListContainer& inst() { static GLListContainer i; return i; }

    ~GLListContainer();

    void drawCircle() const { glCallList(m_circleList); }
private:
    GLListContainer();

    GLuint m_circleList;
};

}
