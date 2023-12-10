export module Widgets.Graph.SmoothMirrorLineGraph;

import Rendering.VBO;

import Widgets.Graph.SmoothLineGraph;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class SmoothMirrorLineGraph {
public:
    SmoothMirrorLineGraph(size_t numGraphSamples);

    void addTopPoint(float valueY);
    void addBottomPoint(float valueY);

    void setTopPoints(const std::vector<float>& values);
    void setBottomPoints(const std::vector<float>& values);

    void resetPoints(size_t numGraphSamples);

    void draw() const;

private:
    SmoothLineGraph m_topGraph;
    SmoothLineGraph m_bottomGraph;
};

}
