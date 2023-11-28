export module Widgets.LineGraph;

import Colors;

import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class LineGraph {
public:
    LineGraph(size_t numGraphSample);

    void updatePoints(const std::vector<GLfloat>& values);
    void draw() const;

private:
    void initPointsVBO();

    void drawPoints() const;

    GraphGrid m_graphGrid;
    OwningVBO<glm::vec2> m_graphPointsVBO;
};

}

