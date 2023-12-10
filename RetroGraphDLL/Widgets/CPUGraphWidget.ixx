export module Widgets.CPUGraphWidget;

import UserSettings;

import Measures.CPUMeasure;

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.Graph.SmoothLineGraph;

import std.memory;

namespace rg {

export class CPUGraphWidget : public Widget {
public:
    CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure);
    ~CPUGraphWidget();

    void draw() const override;

private:
    CPUUsageCallbackHandle RegisterCPUUsageCallback();
    ConfigRefreshedCallbackHandle RegisterConfigRefreshedCallback();

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };
    CPUUsageCallbackHandle m_onCPUUsageHandle;
    ConfigRefreshedCallbackHandle m_configRefreshedHandle;
    int m_graphSampleSize;
    SmoothLineGraph m_graph;
};

}
