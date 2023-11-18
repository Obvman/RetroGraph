export module Widgets.TimeWidget;

import IRetroGraph; // Reverse Dependency

import Measures.CPUMeasure;
import Measures.NetMeasure;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               const IRetroGraph& rg, bool visible) :
        Widget{ fontManager, visible },
        m_cpuMeasure{ &rg.getCPUMeasure() },
        m_netMeasure{ &rg.getNetMeasure() } { }

    ~TimeWidget() noexcept = default;
    TimeWidget(const TimeWidget&) = delete;
    TimeWidget& operator=(const TimeWidget&) = delete;
    TimeWidget(TimeWidget&&) = delete;
    TimeWidget& operator=(TimeWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;

    void draw() const override;
private:
    const CPUMeasure* m_cpuMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
};

} // namespace rg
