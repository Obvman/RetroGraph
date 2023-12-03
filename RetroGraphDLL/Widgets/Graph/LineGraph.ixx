export module Widgets.Graph.LineGraph;

import Colors;

import Rendering.Shader;
import Rendering.VAO;
import Rendering.VBO;

import Widgets.Graph.GraphPointBuffer;
import Widgets.Graph.Spline;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

export class LineGraph {
public:
    explicit LineGraph(size_t numGraphSamples = 0);

    virtual void updatePoints(const std::vector<GLfloat>& values);
    void draw() const;

protected:
    virtual void drawPoints() const;

    VAO m_graphVAO;
    VBO m_graphVerticesVBO;
    Shader m_shader;

    GraphPointBuffer m_pointBuffer;

private:
    void initPointsVBO();
};

constexpr auto defaultPrecisionPoints = size_t{ 100 };

export class SmoothLineGraph : public LineGraph {
public:
    explicit SmoothLineGraph(size_t precisionPoints = defaultPrecisionPoints);

    void updatePoints(const std::vector<GLfloat>& values) override;

private:
    size_t m_precisionPoints;
    tk::spline m_spline;
};

}

