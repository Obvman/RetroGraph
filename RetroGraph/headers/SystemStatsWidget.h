#pragma once

#include <string>
#include <vector>

#include "drawUtils.h"

namespace rg {

class FontManager;
class SystemMeasure;
class CPUMeasure;
class NetMeasure;

class SystemStatsWidget {
public:
    SystemStatsWidget(const FontManager* fontManager, const SystemMeasure* sysInfo,
                      const CPUMeasure* cpuMeasure, const NetMeasure* netMeasure);
    ~SystemStatsWidget() noexcept = default;
    SystemStatsWidget(const SystemStatsWidget&) = delete;
    SystemStatsWidget& operator=(const SystemStatsWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void needsRedraw() const { m_needsRedraw = true; }
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    std::vector<std::string> m_statsStrings{ };

    mutable bool m_needsRedraw{ true };
};

}
