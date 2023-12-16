export module Widgets.CPUGraphWidget;

import UserSettings;

import Measures.CPUMeasure;

import RG.Rendering;

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
    CPUUsageEvent::Handle RegisterCPUUsageCallback();
    ConfigRefreshedEvent::Handle RegisterConfigRefreshedCallback();

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };
    CPUUsageEvent::Handle m_onCPUUsageHandle;
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
    int m_graphSampleSize;
    SmoothLineGraph m_graph;
};

}
