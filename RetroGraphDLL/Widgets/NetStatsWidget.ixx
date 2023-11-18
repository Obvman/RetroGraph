export module Widgets.NetStatsWidget;

import IRetroGraph; // Reverse Dependency

import Measures.NetMeasure;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class NetStatsWidget : public Widget {
public:
    NetStatsWidget(const FontManager* fontManager,
                  const IRetroGraph& rg, bool visible);

    ~NetStatsWidget() noexcept = default;

    NetStatsWidget(const NetStatsWidget&) = delete;
    NetStatsWidget& operator=(const NetStatsWidget&) = delete;
    NetStatsWidget(NetStatsWidget&&) = delete;
    NetStatsWidget& operator=(NetStatsWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override { m_netMeasure = &rg.getNetMeasure();  }
    void draw() const override;

private:
    const NetMeasure* m_netMeasure;
    std::vector<std::string> m_statsStrings;
};

} // namespace rg
