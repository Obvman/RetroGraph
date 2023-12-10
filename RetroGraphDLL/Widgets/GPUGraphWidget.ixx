export module Widgets.GPUGraphWidget;

import UserSettings;

import Measures.GPUMeasure;

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.Graph.SmoothLineGraph;

import std.memory;

namespace rg {

export class GPUGraphWidget : public Widget {
public:
    GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure);
    ~GPUGraphWidget();

    void draw() const override;

private:
    GPUUsageCallbackHandle RegisterGPUUsageCallback();
    ConfigRefreshedCallbackHandle RegisterConfigRefreshedCallback();

    std::shared_ptr<const GPUMeasure> m_gpuMeasure{ nullptr };
    GPUUsageCallbackHandle m_onGPUUsageHandle;
    ConfigRefreshedCallbackHandle m_configRefreshedHandle;
    int m_graphSampleSize;
    SmoothLineGraph m_graph;
};

}
