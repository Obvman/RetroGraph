#pragma once

#include <string>
#include <vector>

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class SystemInfo;
class CPUMeasure;
class NetMeasure;

class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget();
    virtual ~SystemStatsWidget();
    SystemStatsWidget(const SystemStatsWidget&) = delete;
    SystemStatsWidget& operator=(const SystemStatsWidget&) = delete;

    virtual void draw() const override;

    void init(const FontManager* fontManager, const SystemInfo* sysInfo,
              const CPUMeasure* cpuMeasure, const NetMeasure* netMeasure);

    void setViewport(Viewport vp) { m_viewport = vp; };

private:

    const FontManager* m_fontManager;
    Viewport m_viewport;
    std::vector<std::string> m_statsStrings;

};

}
