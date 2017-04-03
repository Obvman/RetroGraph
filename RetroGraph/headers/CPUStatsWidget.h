#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;

class CPUStatsWidget {
public:
    CPUStatsWidget(const FontManager* fontManager,
                   const CPUMeasure* cpuMeasure) :
        m_fontManager{ fontManager }, m_cpuMeasure{ cpuMeasure } {}

    ~CPUStatsWidget() noexcept = default;
    CPUStatsWidget(const CPUStatsWidget&) = delete;
    CPUStatsWidget& operator=(const CPUStatsWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp);

private:
    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };

    const CPUMeasure* m_cpuMeasure{ nullptr };
};

}
