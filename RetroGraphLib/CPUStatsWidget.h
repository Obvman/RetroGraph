#pragma once

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;
class CPUMeasure;

/* Responsible for drawing widget containing CPU voltage, frequency and temperature
 * obtained from CoreTemp 
 */
class CPUStatsWidget : public Widget {
public:
    CPUStatsWidget(const FontManager* fontManager, const CPUMeasure* cpuMeasure,
                   bool visible) :
        Widget{ fontManager, visible }, m_cpuMeasure{ cpuMeasure } {}

    ~CPUStatsWidget() noexcept = default;
    CPUStatsWidget(const CPUStatsWidget&) = delete;
    CPUStatsWidget& operator=(const CPUStatsWidget&) = delete;
    CPUStatsWidget(CPUStatsWidget&&) = delete;
    CPUStatsWidget& operator=(CPUStatsWidget&&) = delete;

    void draw() const override;
    void setViewport(Viewport vp) override;

private:
    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;

    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };

    const CPUMeasure* m_cpuMeasure{ nullptr };
};

} // namespace rg
