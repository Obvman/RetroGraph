export module Widgets.Graph.LineGraph;

import RG.Rendering;

import Widgets.Graph.GraphPointBuffer;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class LineGraph {
public:
    explicit LineGraph(size_t numPoints);

    virtual void addPoint(float valueY);
    virtual void resetPoints(size_t numPoints);
    virtual void setPoints(const std::vector<float>& values);
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

