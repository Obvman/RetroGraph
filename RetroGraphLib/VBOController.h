#pragma once

#include <GL/GL.h>

#include "drawUtils.h"
#include "GLShaders.h"

#include <vector>
#include <utility>

namespace rg {

class AnimationState;

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

// Container for standard VBO data (no element array)
struct VBOContainer {
    VBOContainer() : VBOContainer{ 0 } { }

    VBOContainer(GLsizei size_)
        : id{ 0 }
        , size{ size_ }
        , data( size_ * 2)  { }

    GLuint id;
    GLsizei size; // Num vertices
    std::vector<GLfloat> data;
};


class VBOID;

class VBOController {
public:
    static VBOController & inst() { static VBOController i; return i; }
    ~VBOController();

    VBOID createGraphLineVBO(size_t numValues);
    VBOID createAnimationVBO(size_t numLines);

    // Assumes values are floating percentages between 0 and 100
    void updateGraphLines(const VBOID& vboID, const std::vector<GLfloat>& values);
    void updateAnimationVBO(const VBOID& vboID, const AnimationState& as);

    void drawGraphGrid() const;
    void drawGraphLines(const VBOID& vboID) const;
    void drawAnimationVBO(const VBOID & vboID, int size) const;
private:
    VBOController();

    void initVBOs();
    void initGraphGridVBO();

    void destroyVBO(const VBOID& vboIdx);

    GLuint m_graphGridVertsID;
    GLuint m_graphGridIndicesID;
    GLsizei m_graphGridIndicesSize;

    GLShader m_particleLineShader;

    std::vector<VBOContainer> m_graphLineVBOData;

    GLuint m_animColors;

    friend class VBOID;
};

// Used to index VBOController::m_graphLineVBOData
// not to be confused with id member of VBOContainer, which contains the openGL
// provided id of the VBO
class VBOID {
public:
    VBOID() : m_id{ -1 } {}
    explicit VBOID(int id) : m_id{ id } {}
    explicit VBOID(size_t id) : m_id{ static_cast<int>(id) } {}
    ~VBOID() {
        VBOController::inst().destroyVBO(*this);
    }

    // Very important that we never copy!
    VBOID(const VBOID&) = delete;
    VBOID& operator=(const VBOID&) = delete;
    VBOID(VBOID&&);
    VBOID& operator=(VBOID&&);

    // Conversion to int
    explicit operator int() const { return m_id; }
    operator bool() const { return m_id != -1; }

private:
    int m_id;
};


}
