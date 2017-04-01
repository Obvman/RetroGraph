#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;

class CPUStatsWidget : public Widget {
public:
    CPUStatsWidget() = default;
    virtual ~CPUStatsWidget() noexcept = default;
    CPUStatsWidget(const CPUStatsWidget&) = delete;
    CPUStatsWidget& operator=(const CPUStatsWidget&) = delete;

    virtual void draw() const override;

    void init(const FontManager* fontManager, const CPUMeasure* cpuMeasure);

    void setViewport(Viewport vp);

private:
    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;

    const FontManager* m_fontManager;
    Viewport m_viewport;
    Viewport m_coreGraphViewport;
    Viewport m_statsViewport;

    const CPUMeasure* m_cpuMeasure;
};

}
