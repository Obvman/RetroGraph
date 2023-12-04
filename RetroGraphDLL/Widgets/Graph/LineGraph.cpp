module Widgets.Graph.LineGraph;

import Colors;

import Rendering.DrawUtils;
import Rendering.GLListContainer;

import Widgets.Graph.GraphGrid;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

LineGraph::LineGraph(size_t numGraphSamples)
    : m_graphVerticesVBO{ GL_ARRAY_BUFFER, GL_STREAM_DRAW }
    , m_pointBuffer{ numGraphSamples }
    , m_shader{ "lineGraph" } {

    initPointsVBO();
}

void LineGraph::draw() const {
    GLListContainer::inst().drawBorder();
    GraphGrid::inst().draw();
    drawPoints();
}

void LineGraph::updatePoints(const std::vector<float>& values) {
    auto vboScope{ m_graphVerticesVBO.bind() };

    // Value vectors can change size (infrequently).
    // In this case we need to reallocate buffer data instead of just writing to it
    if (m_pointBuffer.numPoints() != values.size()) {
        std::vector<float> normalizedValues{ values.cbegin(), values.cend() };
        for (auto& value : normalizedValues)
            value = percentageToVP(value);

        m_pointBuffer.setPoints(normalizedValues);
        m_graphVerticesVBO.bufferData(m_pointBuffer.bufferSize() * sizeof(glm::vec2), m_pointBuffer.data());
    } else {
        if (m_pointBuffer.pushPoint(percentageToVP(values.back()))) {
            // All of the points have changed in this case so update the whole range of points in the VBO
            m_graphVerticesVBO.bufferSubData(m_pointBuffer.tail() * sizeof(glm::vec2),
                                             m_pointBuffer.numPoints() * sizeof(glm::vec2), m_pointBuffer.front());
        } else {
            // Only one point has changed here
            m_graphVerticesVBO.bufferSubData(m_pointBuffer.head() * sizeof(glm::vec2),
                                             sizeof(glm::vec2), m_pointBuffer.back());
        }
    }
}

void LineGraph::initPointsVBO() {
    constexpr GLuint vertexLocationIndex{ 0 };

    auto vaoScope{ m_graphVAO.bind() };
    auto vboScope{ m_graphVerticesVBO.bind() };

    glEnableVertexAttribArray(vertexLocationIndex);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    m_graphVerticesVBO.bufferData(m_pointBuffer.bufferSize() * sizeof(glm::vec2), m_pointBuffer.data());
}

void LineGraph::drawPoints() const {
    auto shaderScope{ m_shader.bind() };
    float const xOffset{ (m_pointBuffer.tail() * -m_pointBuffer.getHorizontalPointInterval()) - 1.0f };

    glUniform1f(m_shader.getUniformLocation("xOffset"), xOffset);
    glUniform4f(m_shader.getUniformLocation("color"), GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);

    auto vaoScope{ m_graphVAO.bind() };
    glDrawArrays(GL_LINE_STRIP, static_cast<GLint>(m_pointBuffer.tail()), m_pointBuffer.numPoints());
}

}
