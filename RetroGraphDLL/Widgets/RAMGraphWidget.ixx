export module RG.Widgets:RAMGraphWidget;

import :Widget;

import ConfigRefreshedEvent;

import RG.Measures;
import RG.Rendering;
import RG.Widgets.Graph;

import std.memory;

namespace rg {

export class RAMGraphWidget : public Widget {
public:
    RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure);
    ~RAMGraphWidget();

    void draw() const override;

private:
    RAMUsageEvent::Handle RegisterRAMUsageCallback();
    ConfigRefreshedEvent::Handle RegisterConfigRefreshedCallback();

    std::shared_ptr<const RAMMeasure> m_ramMeasure{ nullptr };
    RAMUsageEvent::Handle m_onRAMUsageHandle;
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
    int m_graphSampleSize;
    SmoothLineGraph m_graph;
};

}
