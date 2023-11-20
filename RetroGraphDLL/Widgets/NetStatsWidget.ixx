export module Widgets.NetStatsWidget;

import Measures.NetMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.core;
import std.memory;

namespace rg {

export class NetStatsWidget : public Widget {
public:
    NetStatsWidget(const FontManager* fontManager,
                   std::shared_ptr<NetMeasure const>, bool visible);

    ~NetStatsWidget() noexcept = default;

    NetStatsWidget(const NetStatsWidget&) = delete;
    NetStatsWidget& operator=(const NetStatsWidget&) = delete;
    NetStatsWidget(NetStatsWidget&&) = delete;
    NetStatsWidget& operator=(NetStatsWidget&&) = delete;

    void draw() const override;

private:
    std::shared_ptr<NetMeasure const> m_netMeasure;
    std::vector<std::string> m_statsStrings;
};

} // namespace rg
