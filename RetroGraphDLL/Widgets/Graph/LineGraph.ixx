export module Widgets.Graph.LineGraph;

import Rendering.VAO;
import Rendering.VBO;

import Widgets.Graph.GraphPointBuffer;

import std.core;

namespace rg {

export class LineGraph {
public:
    explicit LineGraph(size_t numGraphSamples = 0);

    virtual void updatePoints(const std::vector<float>& values);
    void draw() const;

protected:
    virtual void drawPoints() const;

    VAO m_graphVAO;
    VBO m_graphVerticesVBO;

    GraphPointBuffer m_pointBuffer;

private:
    void initPointsVBO();
};

}

