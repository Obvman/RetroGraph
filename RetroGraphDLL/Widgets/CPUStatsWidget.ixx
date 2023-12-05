export module Widgets.CPUStatsWidget;

import Measures.CPUMeasure;

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.Graph.SmoothLineGraph;

import std.core;

namespace rg {

/* Responsible for drawing widget containing CPU voltage, frequency and temperature
 * obtained from CoreTemp
 */
export class CPUStatsWidget : public Widget {
public:
    CPUStatsWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure);
    ~CPUStatsWidget();

    void draw() const override;
    void setViewport(const Viewport& vp) override;

private:
    auto createCoreGraphs(const CPUMeasure& cpuMeasure);

    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };

    std::vector<SmoothLineGraph> m_coreGraphs;

    PostUpdateCallbackHandle m_postUpdateHandle;
};

} // namespace rg
