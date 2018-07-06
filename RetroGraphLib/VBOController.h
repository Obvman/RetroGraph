#pragma once

#include <GL/GL.h>

#include "drawUtils.h"

#include <vector>
#include <utility>

namespace rg {

// Automatically binds/unbinds given VBOs and executes the function given
template<typename F>
void vboElemArrayDrawScope(GLuint vertID, GLuint indexID, F f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template<typename F>
void vboDrawScope(GLuint vertID, F f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

struct VBOContainer {
    VBOContainer()
        : VBOContainer{ 0 } { }
    VBOContainer(GLsizei size_)
        : id{ 0 }
        , size{ size_ }
        , data{}  { }

    GLuint id;
    GLsizei size;
    std::vector<float> data;
};

class VBOController {
public:
    static VBOController & inst() { static VBOController i; return i; }
    ~VBOController();

    void drawGraphGrid() const;

    void updateGraphLines(int vboID, const std::vector<GLfloat>& values) const;
    void drawGraphLines(int vboID) const;

    int createGraphLineVBO(GLsizei numValues);

private:
    VBOController();

    void initVBOs();

    void initGraphGridVBO();

    // TODO 
    // void updateGraphLineVBO(const std::vector<float>& values);

    GLuint m_graphGridVertsID;
    GLuint m_graphGridIndicesID;
    GLsizei m_graphGridIndicesSize;

    std::vector<VBOContainer> m_graphLineVBOData;
};


}
