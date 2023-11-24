export module Rendering.VBOController;

import Measures.AnimationState; // Invalid dependency

import Rendering.GLShader;
import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

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
    void drawAnimationVBO(VBOID vboId, int size, float aspectRatio) const;
private:
    VBOController();

    void initVBOs();
    void initGraphGridVBO();
    VBO<GLfloat>& getVBO(VBOID vboId);
    const VBO<GLfloat>& getVBO(VBOID vboId) const;

    VBO<GLfloat> m_graphGridVerts;
    GLuint m_graphGridIndicesID{ 0 };
    VBO<GLfloat> m_animColors;
    GLsizei m_graphGridIndicesSize{ 0 };
    GLShader m_animShader{ "particle.vert", "particle.frag" };

    GLuint m_animVAOID;

    std::vector<VBO<GLfloat>> m_vbos;
};

}
