export module Widgets.RAMGraphWidget;

import Measures.RAMMeasure;

import Rendering.FontManager;

import Widgets.LineGraph;
import Widgets.Widget;

import std.memory;

namespace rg {

export class RAMGraphWidget : public Widget {
public:
    RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure);
    ~RAMGraphWidget();

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const RAMMeasure> m_ramMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
    LineGraph m_graph;
};

}
