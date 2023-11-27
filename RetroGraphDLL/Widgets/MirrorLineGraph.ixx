export module Widgets.MirrorLineGraph;

import Rendering.VBO;

import Widgets.GraphGrid;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class MirrorLineGraph {
public:
    MirrorLineGraph(size_t numTopGraphSamples, size_t numBottomGraphSamples);

    void updatePoints(const std::vector<GLfloat>& topValues, const std::vector<GLfloat>& bottomValues);
    void draw() const;

private:
    void initPointsVBO(OwningVBO<glm::vec2>& vbo);
    void updatePoints(OwningVBO<glm::vec2>& vbo, const std::vector<GLfloat>& values);

    void drawPoints(const OwningVBO<glm::vec2>& vbo) const;

    GraphGrid m_graphGrid;

    OwningVBO<glm::vec2> m_topGraphPointsVBO;
    OwningVBO<glm::vec2> m_bottomGraphPointsVBO;
};

}
