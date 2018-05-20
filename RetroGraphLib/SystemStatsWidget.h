#pragma once

#include "stdafx.h"

#include <string>
#include <vector>

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class SystemMeasure;
class CPUMeasure;
class NetMeasure;

class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager,
                      const RetroGraph& rg, bool visible);
    ~SystemStatsWidget() noexcept = default;
    SystemStatsWidget(const SystemStatsWidget&) = delete;
    SystemStatsWidget& operator=(const SystemStatsWidget&) = delete;
    SystemStatsWidget(SystemStatsWidget&&) = delete;
    SystemStatsWidget& operator=(SystemStatsWidget&&) = delete;

    void updateObservers(const RetroGraph&) override { /* Empty */ }
    void draw() const override;

    void needsRedraw() const { m_needsRedraw = true; }
private:
    std::vector<std::string> m_statsStrings{ };

    mutable bool m_needsRedraw{ true };
};

} // namespace rg
