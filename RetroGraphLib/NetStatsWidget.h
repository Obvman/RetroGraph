#pragma once

#include "Widget.h"

#include "RetroGraph.h"

#include <vector>
#include <string>

namespace rg {

class FontManager;

class NetStatsWidget : public Widget {
public:
    NetStatsWidget(const FontManager* fontManager,
                  const RetroGraph& rg, bool visible);

    ~NetStatsWidget() noexcept = default;

    NetStatsWidget(const NetStatsWidget&) = delete;
    NetStatsWidget& operator=(const NetStatsWidget&) = delete;
    NetStatsWidget(NetStatsWidget&&) = delete;
    NetStatsWidget& operator=(NetStatsWidget&&) = delete;

    void updateObservers(const RetroGraph& rg) override { m_netMeasure = &rg.getNetMeasure();  }
    void draw() const override;

private:
    const NetMeasure* m_netMeasure;
    std::vector<std::string> m_statsStrings;
};


}
