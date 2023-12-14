export module Widgets.NetStatsWidget;

import Measures.NetMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.core;
import std.memory;

namespace rg {

export class NetStatsWidget : public Widget {
public:
    NetStatsWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure);
    ~NetStatsWidget() noexcept;

    void draw() const override;

private:
    PostUpdateEvent::Handle RegisterPostUpdateCallback();

    std::shared_ptr<const NetMeasure> m_netMeasure;
    std::vector<std::string> m_statsStrings;
    PostUpdateEvent::Handle m_postUpdateHandle;
};

} // namespace rg
