export module Widgets.CPUGraphWidget;

import Measures.CPUMeasure;

import Rendering.FontManager;

import Widgets.LineGraph;
import Widgets.Widget;

import std.memory;

namespace rg {

export class CPUGraphWidget : public Widget {
public:
    CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure);
    ~CPUGraphWidget();

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
    LineGraph m_graph;
};

}
