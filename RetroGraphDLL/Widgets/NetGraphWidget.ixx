export module Widgets.NetGraphWidget;

import Measures.NetMeasure;

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.Graph.SmoothMirrorLineGraph;

import std.memory;

namespace rg {

export class NetGraphWidget : public Widget {
public:
    NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure);
    ~NetGraphWidget();

    void draw() const override;

private:
    std::string getScaleLabel(int64_t bytesTransferred) const;
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const NetMeasure> m_netMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
    SmoothMirrorLineGraph m_netGraph;
    int64_t m_maxDownValue;
    int64_t m_maxUpValue;
};

}
