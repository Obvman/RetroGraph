#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;

class CPUStatsWidget {
public:
    CPUStatsWidget(const FontManager* fontManager, const CPUMeasure* cpuMeasure,
                   bool visible) :
        m_fontManager{ fontManager }, m_visible{ visible },
        m_cpuMeasure{ cpuMeasure } {}

    ~CPUStatsWidget() noexcept = default;
    CPUStatsWidget(const CPUStatsWidget&) = delete;
    CPUStatsWidget& operator=(const CPUStatsWidget&) = delete;

    void draw() const;
    void clear() const;

    void setViewport(Viewport vp);

    void setVisibility(bool b);
private:
    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };
    bool m_visible{ true };

    const CPUMeasure* m_cpuMeasure{ nullptr };
};

}
