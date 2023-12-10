export module Widgets.RAMGraphWidget;

import UserSettings;

import Measures.RAMMeasure;

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.Graph.SmoothLineGraph;

import std.memory;

namespace rg {

export class RAMGraphWidget : public Widget {
public:
    RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure);
    ~RAMGraphWidget();

    void draw() const override;

private:
    RAMUsageCallbackHandle RegisterRAMUsageCallback();
    ConfigRefreshedCallbackHandle RegisterConfigRefreshedCallback();

    std::shared_ptr<const RAMMeasure> m_ramMeasure{ nullptr };
    RAMUsageCallbackHandle m_onRAMUsageHandle;
    ConfigRefreshedCallbackHandle m_configRefreshedHandle;
    int m_graphSampleSize;
    SmoothLineGraph m_graph;
};

}
