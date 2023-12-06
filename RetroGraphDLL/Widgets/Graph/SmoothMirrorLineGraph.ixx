export module Widgets.Graph.SmoothMirrorLineGraph;

import Rendering.VBO;

import Widgets.Graph.SmoothLineGraph;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class SmoothMirrorLineGraph {
public:
    SmoothMirrorLineGraph(size_t numTopGraphSamples, size_t numBottomGraphSamples);

    void updatePoints(const std::vector<float>& topValues, const std::vector<float>& bottomValues);
    void resetPoints(const std::vector<float>& topValues, const std::vector<float>& bottomValues);
    void draw() const;

private:
    SmoothLineGraph m_topGraph;
    SmoothLineGraph m_bottomGraph;
};

}
