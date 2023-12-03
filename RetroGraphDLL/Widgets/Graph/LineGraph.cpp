module Widgets.Graph.LineGraph;

import Rendering.DrawUtils;
import Rendering.GLListContainer;

import Widgets.Graph.GraphGrid;
import Widgets.Graph.Spline;

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

void LineGraph::updatePoints(const std::vector<GLfloat>& values) {
    auto vboScope{ m_graphVerticesVBO.bind() };

    // Value vectors can change size (infrequently).
    // In this case we need to reallocate buffer data instead of just writing to it
    if (m_pointBuffer.numPoints() != values.size()) {
        m_pointBuffer.setPoints(values);
        m_graphVerticesVBO.bufferData(m_pointBuffer.bufferSize() * sizeof(glm::vec2), m_pointBuffer.data());
    } else {
        if (m_pointBuffer.pushPoint(values.back())) {
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
    glUniform4f(m_shader.getUniformLocation("color"), GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.5f);
    if (dynamic_cast<SmoothLineGraph const*>(this))
        glUniform4f(m_shader.getUniformLocation("color"), PINK1_R, PINK1_G, PINK1_B, 1.0f);

    auto vaoScope{ m_graphVAO.bind() };

    glDrawArrays(GL_LINE_STRIP, static_cast<GLint>(m_pointBuffer.tail()), static_cast<GLsizei>(m_pointBuffer.numPoints()));
}

SmoothLineGraph::SmoothLineGraph(size_t precisionPoints)
    : LineGraph{ precisionPoints }
    , m_precisionPoints{ precisionPoints }
    , m_spline{} {
}

void SmoothLineGraph::updatePoints(const std::vector<GLfloat>& values) {
    // Rebuild the last few segments of the spline. We only need to rebuild a
    // few segments since the earlier parts of the spline are not modified by adding a new point.
    std::vector<float> rawX;
    std::vector<float> rawY;
    for (int i = std::min(0, static_cast<int>(values.size()) - 4); i < values.size(); ++i) {
        rawX.push_back(percentageToVP(static_cast<GLfloat>(i) / (values.size() - 1)));
        rawY.push_back(values[i]);
    }
    m_spline.set_points(rawX, rawY, tk::spline::cspline_hermite);

    // Changing one point in a hermite spline affects two segments to the left and right of the
    // point that changed. We need to recalculate those points and update them in the buffer and VBO.
    // Since we always append to the end of the graph buffer we only need to recalculate the two segments
    // to the left of the new point.
    int numPointsInSegment{ static_cast<int>(std::floor(static_cast<float> (m_precisionPoints) / values.size())) };
    float segmentWidth{ viewportWidth / values.size() };
    float splineSegmentStep{ segmentWidth / numPointsInSegment };

    // Overwrite the existing points in the spline that may have their shape be modified by the new point
    float oldSegmentStart{ viewportMax - (2 * segmentWidth) };
    // Start at 1, since 0 would be the last point of the previous segment we already calculated.
    for (int i = 1; i <= numPointsInSegment; ++i) {
        float const splineX{ oldSegmentStart + (i * splineSegmentStep) };
        float const splineY{ m_spline(splineX) };
        int oldPointIndex{ static_cast<int>(m_pointBuffer.head()) - numPointsInSegment + i};
        m_pointBuffer[oldPointIndex].y = clampToViewport(percentageToVP(splineY));
    }

    // Add the new points in the spline
    float newSegmentStart{ viewportMax - segmentWidth };
    bool didBufferReallocate{ false };
    // Start at 1, since 0 would be the last point of the previous segment we already calculated.
    for (int i = 1; i <= numPointsInSegment; ++i) {
        float splineX{ newSegmentStart + (i * splineSegmentStep) };
        float const splineY{ clampToViewport(m_spline(splineX)) };
        didBufferReallocate &= m_pointBuffer.pushPoint(splineY);
    }

    auto vboScope{ m_graphVerticesVBO.bind() };
    if (didBufferReallocate) {
        // All of the points have changed in this case so update the whole range of points in the VBO
        m_graphVerticesVBO.bufferSubData(m_pointBuffer.tail() * sizeof(glm::vec2),
                                         m_pointBuffer.numPoints() * sizeof(glm::vec2), m_pointBuffer.front());
    } else {
        // TODO update the whole segment
        //m_graphVerticesVBO.bufferSubData(m_pointBuffer.head() * sizeof(glm::vec2),
        //                                 sizeof(glm::vec2), m_pointBuffer.back());
        m_graphVerticesVBO.bufferSubData(m_pointBuffer.tail() * sizeof(glm::vec2),
                                         m_pointBuffer.numPoints() * sizeof(glm::vec2), m_pointBuffer.front());
    }
}

}
