export module Widgets.GraphGrid;

import Rendering.VBO;

import "GLHeaderUnit.h";

namespace rg {

export class GraphGrid {
public:
    GraphGrid();

    void draw() const;
private:
    void initVBO();

    OwningVBO<glm::vec2> m_graphGridVerts;
    OwningVBO<GLuint> m_graphGridIndices;
};

}
