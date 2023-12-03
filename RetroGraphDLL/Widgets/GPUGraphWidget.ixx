export module Widgets.GPUGraphWidget;

import Measures.GPUMeasure;

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.Graph.LineGraph;

import std.memory;

namespace rg {

export class GPUGraphWidget : public Widget {
public:
    GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure);
    ~GPUGraphWidget();

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const GPUMeasure> m_gpuMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
    LineGraph m_graph;
};

}
