export module Widgets.CPUStatsWidget;

import UserSettings;

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
    CPUCoreUsageCallbackHandle RegisterOnCPUCoreUsageCallback();
    CPUCoreDataStateChangedCallbackHandle RegisterOnCPUCoreDataStateChangedCallback();
    ConfigRefreshedCallbackHandle RegisterConfigRefreshedCallback();

    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };

    bool m_coreDataAvailable;
    int m_coreGraphSampleSize;
    std::vector<SmoothLineGraph> m_coreGraphs;

    CPUCoreUsageCallbackHandle m_onCPUCoreUsageHandle;
    CPUCoreDataStateChangedCallbackHandle m_onCPUCoreDataStateChangedHandle;
    ConfigRefreshedCallbackHandle m_configRefreshedHandle;
};

} // namespace rg
