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
    SystemStatsWidget(const FontManager* fontManager,
                      const SystemMeasure* sysInfo,
                      const CPUMeasure* cpuMeasure,
                      const NetMeasure* netMeasure,
                      bool visible);
    ~SystemStatsWidget() noexcept = default;
    SystemStatsWidget(const SystemStatsWidget&) = delete;
    SystemStatsWidget& operator=(const SystemStatsWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void setVisible(bool b) { m_visible = b; }

    void needsRedraw() const { m_needsRedraw = true; }
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };
    std::vector<std::string> m_statsStrings{ };

    mutable bool m_needsRedraw{ true };
};

}
