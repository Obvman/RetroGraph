export module Widgets.LineGraph;

import Colors;

import Rendering.Shader;
import Rendering.VAO;
import Rendering.VBO;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

class GraphPointBuffer {
public:
    size_t bufferSize() const { return m_rollingBuffer.size(); }
    size_t numPoints() const { return bufferSize() / 2; }
    float getHorizontalPointInterval() const { return 2.0f / (numPoints() - 1); }

private:
    std::vector<glm::vec2> m_rollingBuffer;
    size_t m_head;
    size_t m_tail;
};

export class LineGraph {
public:
    explicit LineGraph(size_t numGraphSamples = 0);

    virtual void updatePoints(const std::vector<GLfloat>& values);
    void draw() const;

protected:
    virtual void drawPoints() const;

    size_t bufferSize() const { return m_rollingBuffer.size(); }
    size_t numPoints() const { return bufferSize() / 2; }
    float getHorizontalPointInterval() const { return 2.0f / (numPoints() - 1); }

    VAO m_graphVAO;
    VBO m_graphVerticesVBO;
    Shader m_shader;

    std::vector<glm::vec2> m_rollingBuffer;
    size_t m_head;
    size_t m_tail;
private:
    void initPointsVBO();
};

constexpr auto defaultPrecisionPoints = size_t{ 300 };

export class SmoothLineGraph : public LineGraph {
public:
    explicit SmoothLineGraph(size_t precisionPoints = defaultPrecisionPoints);

    void updatePoints(const std::vector<GLfloat>& values) override;

private:
    size_t m_precisionPoints;
};

}

