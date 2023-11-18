export module Rendering.VBOController;

import Measures.AnimationState; // Invalid dependency

import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export using VBOID = size_t;

export class VBOController {
public:
    static VBOController & inst() { static VBOController i; return i; }
    ~VBOController();

    VBOID createGraphLineVBO(size_t numValues);
    VBOID createAnimationVBO(size_t numLines);

    // Assumes values are floating percentages between 0 and 100
    void updateGraphLines(VBOID vboId, const std::vector<GLfloat>& values);
    void updateAnimationVBO(VBOID vboId, const AnimationState& as);

    void drawGraphGrid() const;
    void drawGraphLines(VBOID vboId) const;
    void drawAnimationVBO(VBOID vboId, int size) const;
private:
    VBOController();

    void initVBOs();
    void initGraphGridVBO();
    VBO& getVBO(VBOID vboId);
    const VBO& getVBO(VBOID vboId) const;

    GLuint m_graphGridVertsID{ 0 };
    GLuint m_graphGridIndicesID{ 0 };
    GLuint m_animColors{ 0 };
    GLsizei m_graphGridIndicesSize{ 0 };

    std::vector<VBO> m_vbos;
};

}
