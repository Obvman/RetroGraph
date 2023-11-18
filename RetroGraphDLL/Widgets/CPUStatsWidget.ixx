export module Widgets.CPUStatsWidget;

import IRetroGraph; // Reverse Dependency

import Measures.CPUMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.core;

namespace rg {

/* Responsible for drawing widget containing CPU voltage, frequency and temperature
 * obtained from CoreTemp
 */
export class CPUStatsWidget : public Widget {
public:
    CPUStatsWidget(const FontManager* fontManager, const IRetroGraph& rg,
                   bool visible) :
        Widget{ fontManager, visible },
        m_cpuMeasure{ &rg.getCPUMeasure() } {}

    ~CPUStatsWidget() noexcept = default;
    CPUStatsWidget(const CPUStatsWidget&) = delete;
    CPUStatsWidget& operator=(const CPUStatsWidget&) = delete;
    CPUStatsWidget(CPUStatsWidget&&) = delete;
    CPUStatsWidget& operator=(CPUStatsWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;
    void setViewport(const Viewport& vp) override;

private:
    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;

    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };

    const CPUMeasure* m_cpuMeasure{ nullptr };
};

} // namespace rg
