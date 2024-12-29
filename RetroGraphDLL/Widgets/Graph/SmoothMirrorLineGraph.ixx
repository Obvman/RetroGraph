export module RG.Widgets.Graph:SmoothMirrorLineGraph;

import :SmoothLineGraph;

import RG.Rendering;

import std;

import "GLHeaderUnit.h";

namespace rg {

export class SmoothMirrorLineGraph {
public:
    SmoothMirrorLineGraph(size_t numGraphSamples);

    void draw() const;

    void resetPoints(size_t numGraphSamples);

    SmoothLineGraph& topGraph() { return m_topGraph; }
    const SmoothLineGraph& topGraph() const { return m_topGraph; }

    SmoothLineGraph& bottomGraph() { return m_bottomGraph; }
    const SmoothLineGraph& bottomGraph() const { return m_bottomGraph; }

private:
    SmoothLineGraph m_topGraph;
    SmoothLineGraph m_bottomGraph;
};

} // namespace rg
