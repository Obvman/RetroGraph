export module Widgets.Graph.GraphGrid;

import Rendering.VBO;

import "GLHeaderUnit.h";

namespace rg {

export class GraphGrid {
public:
    // GraphGrid never changes so it can be shared across all Graphs as a static instance so we don't have
    // to generate VBOs for each case.
    static const GraphGrid& inst() { static GraphGrid instance; return instance; }

    void draw() const;
private:
    GraphGrid();

    void initVBO();

    OwningVBO<glm::vec2> m_graphGridVerts;
    OwningVBO<GLuint> m_graphGridIndices;
};

}
