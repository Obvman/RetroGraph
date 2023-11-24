export module Rendering.VBOController;

import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class VBOController {
public:
    static VBOController & inst() { static VBOController i; return i; }
    ~VBOController();

    VBOID createGraphLineVBO(size_t numValues);

    // Assumes values are floating percentages between 0 and 100
    void updateGraphLines(VBOID vboId, const std::vector<GLfloat>& values);

    void drawGraphGrid() const;
    void drawGraphLines(VBOID vboId) const;

private:
    VBOController();

    void initVBOs();
    void initGraphGridVBO();
    VBO<glm::vec2>& getVBO(VBOID vboId);
    const VBO<glm::vec2>& getVBO(VBOID vboId) const;

    VBO<GLfloat> m_graphGridVerts{ GL_ARRAY_BUFFER };
    VBO<GLuint> m_graphGridIndices{ GL_ELEMENT_ARRAY_BUFFER };

    std::vector<VBO<glm::vec2>> m_vbos;
};

}
