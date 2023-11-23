export module Widgets.CPUStatsWidget;

import Measures.CPUMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.core;

namespace rg {

/* Responsible for drawing widget containing CPU voltage, frequency and temperature
 * obtained from CoreTemp
 */
export class CPUStatsWidget : public Widget {
public:
    CPUStatsWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure) :
        Widget{ fontManager },
        m_cpuMeasure{ cpuMeasure } {}

    ~CPUStatsWidget() noexcept = default;

    void draw() const override;
    void setViewport(const Viewport& vp) override;

private:
    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;

    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };
};

} // namespace rg