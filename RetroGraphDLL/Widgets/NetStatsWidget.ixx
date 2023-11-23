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
    ~NetStatsWidget() noexcept = default;

    void draw() const override;

private:
    std::shared_ptr<const NetMeasure> m_netMeasure;
    std::vector<std::string> m_statsStrings;
};

} // namespace rg