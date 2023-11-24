export module Rendering.VBOController;

import Measures.AnimationState; // Invalid dependency
import Measures.ParticleLine; // Invalid dependency

import Rendering.GLShader;
import Rendering.VAO;
import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class VBOController {
public:
    static VBOController & inst() { static VBOController i; return i; }
    ~VBOController();

    void reloadShaders();

    VBOID createGraphLineVBO(size_t numValues);
    void createAnimationVBO(size_t numLines);

    // Assumes values are floating percentages between 0 and 100
    void updateGraphLines(VBOID vboId, const std::vector<GLfloat>& values);
    void updateAnimationVBO(const AnimationState& as);

    void drawGraphGrid() const;
    void drawGraphLines(VBOID vboId) const;
    void drawAnimationVBO(int size, float aspectRatio) const;

private:
    VBOController();

    void initVBOs();
    void initGraphGridVBO();
    VBO<glm::vec2>& getVBO(VBOID vboId);
    const VBO<glm::vec2>& getVBO(VBOID vboId) const;

    VBO<GLfloat> m_graphGridVerts{ GL_ARRAY_BUFFER };
    VBO<GLuint> m_graphGridIndices{ GL_ELEMENT_ARRAY_BUFFER };

    VAO m_animVAO;
    VBO<ParticleLine> m_animLines{ GL_ARRAY_BUFFER };
    GLShader m_animShader{ "particleLine" };

    std::vector<VBO<glm::vec2>> m_vbos;
};

}
