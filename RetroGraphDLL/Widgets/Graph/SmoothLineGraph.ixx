export module RG.Widgets.Graph:SmoothLineGraph;

import :LineGraph;
import :Spline;

import std.core;

namespace rg {

constexpr auto defaultPrecisionPoints = size_t{ 5 };

export class SmoothLineGraph : public LineGraph {
public:
    explicit SmoothLineGraph(size_t numGraphSamples);

    void addPoint(float valueY) override;
    void resetPoints(size_t numPoints) override;
    void setPoints(const std::vector<float>& values) override;

private:
    size_t m_numSamples;
    int m_precisionPoints;
    tk::spline m_spline;
};

}
