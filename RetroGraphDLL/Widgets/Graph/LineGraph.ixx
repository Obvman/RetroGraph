export module Widgets.Graph.LineGraph;

import Rendering.VAO;
import Rendering.VBO;

import Widgets.Graph.GraphPointBuffer;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class LineGraph {
public:
    explicit LineGraph(size_t numGraphSamples = 0);

    virtual void updatePoints(const std::vector<float>& values);
    virtual void resetPoints(const std::vector<float>& values);
    void draw() const;

    void setModelView(const glm::mat4& modelView) { m_modelView = modelView; }
    void setDrawDecorations(bool drawDecorations) { m_drawDecorations = drawDecorations; }

protected:
    virtual void drawPoints() const;

    VAO m_graphVAO;
    VBO m_graphVerticesVBO;

    GraphPointBuffer m_pointBuffer;

private:
    void initPointsVBO();

    bool m_drawDecorations;
    glm::mat4 m_modelView;
};

}

