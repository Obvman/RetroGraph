module RG.Widgets.Graph:SmoothLineGraph;

import RG.Rendering;

import "RGAssert.h";

namespace rg {

constexpr int splinePointCacheSize{ 4 };

size_t getNumberOfCurvePoints(size_t numGraphSamples) {
    return (defaultPrecisionPoints - 1) * (numGraphSamples - 1) + 1;
}

SmoothLineGraph::SmoothLineGraph(size_t numGraphSamples)
    : LineGraph{ getNumberOfCurvePoints(numGraphSamples) }
    , m_numSamples{ numGraphSamples }
    , m_precisionPoints{ defaultPrecisionPoints }
    , m_spline{} {
    RGASSERT(m_numSamples > splinePointCacheSize, "Not enough sample points for a smooth graph");
    resetPoints(m_numSamples);
}

void SmoothLineGraph::addPoint(float valueY) {
    // Rebuild the last few segments of the spline. We only need to rebuild a
    // few segments since the earlier parts of the spline are not modified by adding a new point.
    std::vector<float> splinePointsX{ m_spline.get_x() };
    std::vector<float> splinePointsY{ m_spline.get_y() };
    std::rotate(splinePointsY.begin(), splinePointsY.begin() + 1, splinePointsY.end());
    splinePointsY.back() = percentageToVP(valueY);
    m_spline.set_points(splinePointsX, splinePointsY, tk::spline::cspline_hermite);

    // Changing one point in a hermite spline affects two segments to the left and right of the
    // point that changed. We need to recalculate those points and update them in the buffer and VBO.
    // Since we always append to the end of the graph buffer we only need to recalculate the two segments
    // to the left of the new point.
    float segmentWidth{ viewportWidth / (m_numSamples - 1) };
    float segmentStep{ segmentWidth / (m_precisionPoints - 1) };
    float newSegmentStart{ viewportMax - segmentWidth };
    float oldSegmentStart{ viewportMax - (2 * segmentWidth) };
    bool didBufferReallocate{ false };

    // Add the new points in the spline
    // Start at 1, since 0 would be the last point of the previous segment we already calculated.
    for (int i = 1; i < m_precisionPoints; ++i) {
        float splineX{ newSegmentStart + (i * segmentStep) };
        const float splineY{ clampToViewport(m_spline(splineX)) };
        if (m_pointBuffer.pushPoint(splineY))
            didBufferReallocate = true;
    }

    // Overwrite the existing points in the spline's previous segment
    // that may have their shape be modified by the new point
    int oldSegmentStartIndex{ static_cast<int>(m_pointBuffer.numPoints() - 1) - 2 * (m_precisionPoints - 1) };
    for (int i = 0; i < m_precisionPoints; ++i) {
        const float splineX{ oldSegmentStart + (i * segmentStep) };
        const float splineY{ clampToViewport(m_spline(splineX)) };
        m_pointBuffer[oldSegmentStartIndex + i].y = splineY;
    }

    auto vboScope{ m_graphVerticesVBO.bind() };
    if (didBufferReallocate) {
        // All of the points have changed in this case so update the whole range of points in the VBO
        m_graphVerticesVBO.bufferSubData(m_pointBuffer.tail() * sizeof(glm::vec2),
                                         m_pointBuffer.numPoints() * sizeof(glm::vec2), m_pointBuffer.front());
    } else {
        // update only the final two segments which may have changed
        m_graphVerticesVBO.bufferSubData((m_pointBuffer.tail() + oldSegmentStartIndex) * sizeof(glm::vec2),
                                         (m_precisionPoints * 2) * sizeof(glm::vec2),
                                         &m_pointBuffer[oldSegmentStartIndex]);
    }
}

void SmoothLineGraph::resetPoints(size_t numPoints) {
    m_numSamples = numPoints;
    RGASSERT(m_numSamples > splinePointCacheSize, "Not enough sample points for a smooth graph");

    // Cache four points in the spline which is used to calculate the curves
    // when a new point is added
    std::vector<float> splinePointsX;
    std::vector<float> splinePointsY;
    for (int i{ 0 }; i < splinePointCacheSize; ++i) {
        int valueIndex{ static_cast<int>(m_numSamples) - splinePointCacheSize + i };
        splinePointsX.push_back(percentageToVP(static_cast<float>(valueIndex) / (m_numSamples - 1)));
        splinePointsY.push_back(viewportMin);
    }
    m_spline.set_points(splinePointsX, splinePointsY, tk::spline::cspline_hermite);

    m_pointBuffer = GraphPointBuffer{ getNumberOfCurvePoints(m_numSamples) };

    auto vboScope{ m_graphVerticesVBO.bind() };
    m_graphVerticesVBO.bufferData(m_pointBuffer.bufferSize() * sizeof(glm::vec2), m_pointBuffer.data());
}

void SmoothLineGraph::setPoints(const std::vector<float>& values) {
    m_numSamples = values.size();
    RGASSERT(m_numSamples > splinePointCacheSize, "Not enough sample points for a smooth graph");

    // Build the entire spline so we can recalculate the curve four existing points.
    std::vector<float> splinePointsX;
    std::vector<float> splinePointsY;
    for (int i{ 0 }; i < values.size(); ++i) {
        splinePointsX.push_back(percentageToVP(static_cast<float>(i) / (values.size() - 1)));
        splinePointsY.push_back(percentageToVP(values[i]));
    }
    m_spline.set_points(splinePointsX, splinePointsY, tk::spline::cspline_hermite);

    m_pointBuffer = GraphPointBuffer{ getNumberOfCurvePoints(values.size()) };
    for (int i{ 0 }; i < m_pointBuffer.numPoints(); ++i) {
        const float splineX{ percentageToVP(static_cast<float>(i) / (m_pointBuffer.numPoints() - 1)) };
        const float splineY{ clampToViewport(m_spline(splineX)) };
        m_pointBuffer.pushPoint(splineY);
    }

    auto vboScope{ m_graphVerticesVBO.bind() };
    m_graphVerticesVBO.bufferData(m_pointBuffer.bufferSize() * sizeof(glm::vec2), m_pointBuffer.data());

    // Cut down the spline to the last few points. Future updates will just add a single point to the end and we
    // only need a few points in the spine to update the curves
    std::vector<float> splineEndPointsX{ splinePointsX.cend() - splinePointCacheSize, splinePointsX.cend() };
    std::vector<float> splineEndPointsY{ splinePointsY.cend() - splinePointCacheSize, splinePointsY.cend() };
    m_spline.set_points(splineEndPointsX, splineEndPointsY, tk::spline::cspline_hermite);
}

} // namespace rg
