module Widgets.LineGraph;

import Rendering.DrawUtils;
import Rendering.GLListContainer;

import Widgets.GraphGrid;
import Widgets.Spline;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

LineGraph::LineGraph(size_t numGraphSamples)
    : m_graphVerticesVBO{ GL_ARRAY_BUFFER, GL_STREAM_DRAW }
    , m_rollingBuffer{ numGraphSamples * 2 }
    , m_head{ numGraphSamples - 1 }
    , m_tail{ 0 }
    , m_shader{ "lineGraph" } {

    initPointsVBO();
}

void LineGraph::draw() const {
    GLListContainer::inst().drawBorder();
    GraphGrid::inst().draw();
    drawPoints();
}

void LineGraph::updatePoints(const std::vector<GLfloat>& values) {
    auto vboScope{ m_graphVerticesVBO.bind() };

    // Value vectors can change size (infrequently).
    // In this case we need to reallocate buffer data instead of just writing to it
    if (numPoints() != values.size()) {

        m_rollingBuffer.resize(values.size() * 2);
        m_head = values.size() - 1;
        m_tail = 0;

        for (size_t i = m_tail; i <= m_head; ++i) {
            const auto x{static_cast<GLfloat>(i) / m_head * 2.0f};
            // Copy the values into the first half of the buffer
            m_rollingBuffer[i] = { x, values[i] * 2.0f - 1.0f };

            // Initialize the static x values in the second half of the buffer.
            // y values will be written as the buffer rolls forward
            m_rollingBuffer[i + m_head + 1] = { x + 2.0f + getHorizontalPointInterval(), 0.0f};
        }

        m_graphVerticesVBO.bufferData(bufferSize() * sizeof(glm::vec2), m_rollingBuffer.data());
    } else {
        ++m_head;
        ++m_tail;
        m_rollingBuffer[m_head].y = values.back() * 2.0f - 1.0f;

        // When the head of the rolling buffer hits the end, copy the second half of the buffer into the first half
        // and update the pointers to target the first half of the buffer.
        if (m_head == bufferSize() - 1) {
            for (size_t i = m_tail; i <= m_head; ++i) {
                m_rollingBuffer[i - m_tail].y = m_rollingBuffer[i].y;
            }
            m_head -= m_tail;
            m_tail = 0;

            // All of the points have changed in this case so update the whole range of points in the VBO
            m_graphVerticesVBO.bufferSubData(m_tail * sizeof(glm::vec2), numPoints() * sizeof(glm::vec2), &m_rollingBuffer[m_tail]);
        } else {
            // Only one point has changed here
            m_graphVerticesVBO.bufferSubData(m_head * sizeof(glm::vec2), sizeof(glm::vec2), &m_rollingBuffer[m_head]);
        }
    }
}

void LineGraph::initPointsVBO() {
    constexpr GLuint vertexLocationIndex{ 0 };

    auto vaoScope{ m_graphVAO.bind() };
    auto vboScope{ m_graphVerticesVBO.bind() };

    glEnableVertexAttribArray(vertexLocationIndex);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    m_graphVerticesVBO.bufferData(m_rollingBuffer.size() * sizeof(glm::vec2), m_rollingBuffer.data());
}

void LineGraph::drawPoints() const {
    auto shaderScope{ m_shader.bind() };
    float const xOffset{ (m_tail * -getHorizontalPointInterval()) - 1.0f };

    glUniform1f(m_shader.getUniformLocation("xOffset"), xOffset);
    glUniform4f(m_shader.getUniformLocation("color"), GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);

    auto vaoScope{ m_graphVAO.bind() };

    glDrawArrays(GL_LINE_STRIP, static_cast<GLint>(m_tail), static_cast<GLsizei>(numPoints()));
}

SmoothLineGraph::SmoothLineGraph(size_t precisionPoints)
    : LineGraph{ precisionPoints }
    , m_precisionPoints{ precisionPoints } {
}

void SmoothLineGraph::updatePoints(const std::vector<GLfloat>& values) {
    (void)values;
    //auto vboScope{ m_graphPointsVBO.bind() };
    //auto& verts{ m_graphPointsVBO.data()};

    //RGASSERT(verts.size() == values.size(), "Graph values don't match");

    //std::vector<float> rawX (values.size());
    //std::vector<float> rawY (values.size());
    //for (size_t i = 0; i < values.size(); ++i) {
    //    rawX[i] = (static_cast<GLfloat>(i) / (values.size() - 1)) * 2.0f - 1.0f;
    //    rawY[i] = values[i] * 2.0f - 1.0f;
    //}

    //tk::spline spl(rawX, rawY, tk::spline::cspline_hermite);
    //for (size_t i = 0; i < m_precisionPoints; ++i) {
    //    float const x{ (static_cast<float>(i) / m_precisionPoints) * 2.0f - 1.0f};
    //    //verts[i] = { x, std::clamp(spl(x), -1.0f, 1.0f) };
    //    verts.push({ x, std::clamp(spl(x), -1.0f, 1.0f) });
    //}

    //m_graphPointsVBO.bufferSubData(0, m_graphPointsVBO.sizeBytes());
}

}
