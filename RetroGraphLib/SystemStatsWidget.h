#pragma once

#include <string>
#include <vector>

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;
class SystemMeasure;
class CPUMeasure;
class NetMeasure;

class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager,
                      const SystemMeasure* sysInfo,
                      const CPUMeasure* cpuMeasure,
                      const NetMeasure* netMeasure,
                      bool visible);
    ~SystemStatsWidget() noexcept = default;
    SystemStatsWidget(const SystemStatsWidget&) = delete;
    SystemStatsWidget& operator=(const SystemStatsWidget&) = delete;
    SystemStatsWidget(SystemStatsWidget&&) = delete;
    SystemStatsWidget& operator=(SystemStatsWidget&&) = delete;

    void draw() const override;

    void needsRedraw() const { m_needsRedraw = true; }
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };
    std::vector<std::string> m_statsStrings{ };

    mutable bool m_needsRedraw{ true };
};

} // namespace rg
