export module RG.Widgets:GPUGraphWidget;

import :Widget;

import ConfigRefreshedEvent;

import RG.Measures;
import RG.Rendering;
import RG.Widgets.Graph;

import std.memory;

namespace rg {

export class GPUGraphWidget : public Widget {
public:
    GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure);
    ~GPUGraphWidget();

    void draw() const override;

private:
    GPUUsageEvent::Handle RegisterGPUUsageCallback();
    ConfigRefreshedEvent::Handle RegisterConfigRefreshedCallback();

    std::shared_ptr<const GPUMeasure> m_gpuMeasure{ nullptr };
    GPUUsageEvent::Handle m_onGPUUsageHandle;
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
    int m_graphSampleSize;
    SmoothLineGraph m_graph;
};

}
