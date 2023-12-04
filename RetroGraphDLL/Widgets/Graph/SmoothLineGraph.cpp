module Widgets.Graph.SmoothLineGraph;

import Rendering.DrawUtils;

namespace rg {

SmoothLineGraph::SmoothLineGraph(size_t numGraphSamples)
    : LineGraph{ (defaultPrecisionPoints - 1) * (numGraphSamples - 1) + 1 }
    , m_precisionPoints{ defaultPrecisionPoints }
    , m_spline{} {
}

void SmoothLineGraph::updatePoints(const std::vector<float>& values) {
    // #TODO buffer is not reallocated when changing size of graph samples

    // Rebuild the last few segments of the spline. We only need to rebuild a
    // few segments since the earlier parts of the spline are not modified by adding a new point.
    std::vector<float> rawX;
    std::vector<float> rawY;
    for (int i{ std::max(0, static_cast<int>(values.size()) - 4) }; i < values.size(); ++i) {
        rawX.push_back(percentageToVP(static_cast<float>(i) / (values.size() - 1)));
        rawY.push_back(values[i]);
    }
    m_spline.set_points(rawX, rawY, tk::spline::cspline_hermite);

    // Changing one point in a hermite spline affects two segments to the left and right of the
    // point that changed. We need to recalculate those points and update them in the buffer and VBO.
    // Since we always append to the end of the graph buffer we only need to recalculate the two segments
    // to the left of the new point.
    float segmentWidth{ viewportWidth / (values.size() - 1) };
    float segmentStep{ segmentWidth / (m_precisionPoints - 1) };
    float newSegmentStart{ viewportMax - segmentWidth };
    float oldSegmentStart{ viewportMax - (2 * segmentWidth) };
    bool didBufferReallocate{ false };

    // Add the new points in the spline
    // Start at 1, since 0 would be the last point of the previous segment we already calculated.
    for (int i = 1; i < m_precisionPoints; ++i) {
        float splineX{ newSegmentStart + (i * segmentStep) };
        float const splineY{ clampToViewport(percentageToVP(m_spline(splineX))) };
        if (m_pointBuffer.pushPoint(splineY))
            didBufferReallocate = true;
    }

    // Overwrite the existing points in the spline's previous segment
    // that may have their shape be modified by the new point
    int oldSegmentStartIndex{ static_cast<int>(m_pointBuffer.head()) - 2 * (m_precisionPoints - 1) };
    for (int i = 0; i < m_precisionPoints; ++i) {
        float const splineX{ oldSegmentStart + (i * segmentStep) };
        float const splineY{ clampToViewport(percentageToVP(m_spline(splineX))) };
        m_pointBuffer[oldSegmentStartIndex + i].y = splineY;
    }

    auto vboScope{ m_graphVerticesVBO.bind() };
    if (didBufferReallocate) {
        // All of the points have changed in this case so update the whole range of points in the VBO
        m_graphVerticesVBO.bufferSubData(m_pointBuffer.tail() * sizeof(glm::vec2),
                                         m_pointBuffer.numPoints() * sizeof(glm::vec2), m_pointBuffer.front());
    } else {
        // update only the final two segments which may have changed
        m_graphVerticesVBO.bufferSubData(oldSegmentStartIndex * sizeof(glm::vec2),
                                         (m_pointBuffer.head() - oldSegmentStartIndex + 1) * sizeof(glm::vec2),
                                         &m_pointBuffer[oldSegmentStartIndex]);
    }
}

}
