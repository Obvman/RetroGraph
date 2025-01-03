export module RG.Widgets:CPUStatsWidget;

import :Widget;

import RG.Measures;
import RG.Rendering;
import RG.UserSettings;
import RG.Widgets.Graph;

import std.core;

namespace rg {

/* Responsible for drawing widget containing CPU voltage, frequency and temperature */
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
    CPUCoreUsageEvent::Handle RegisterOnCPUCoreUsageCallback();
    ConfigRefreshedEvent::Handle RegisterConfigRefreshedCallback();

    Viewport m_coreGraphViewport{};
    Viewport m_statsViewport{};

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };

    int m_coreGraphSampleSize;
    std::vector<SmoothLineGraph> m_coreGraphs;

    CPUCoreUsageEvent::Handle m_onCPUCoreUsageHandle;
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
};

} // namespace rg
