export module Widgets.LineGraph;

import Colors;

import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class LineGraph {
public:
    LineGraph(size_t numGraphSamples,
              const glm::vec4& color = { GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A },
              bool drawBackground = true);

    void draw() const;
    void updatePoints(const std::vector<GLfloat>& values) const;

    void setColor(glm::vec4 color) { m_color = color; }

private:
    void initGridVBO();
    void initPointsVBO();

    void drawGrid() const;
    void drawPoints() const;

    OwningVBO<glm::vec2> m_graphGridVerts;
    OwningVBO<GLuint> m_graphGridIndices;
    mutable OwningVBO<glm::vec2> m_graphPointsVBO; // #TODO mutable
    glm::vec4 m_color;
    bool m_drawBackground;
};

}

