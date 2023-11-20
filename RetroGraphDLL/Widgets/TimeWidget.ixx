export module Widgets.TimeWidget;

import Measures.CPUMeasure;
import Measures.NetMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               std::shared_ptr<CPUMeasure const> cpuMeasure,
               std::shared_ptr<NetMeasure const> netMeasure,
               bool visible) :
        Widget{ fontManager, visible },
        m_cpuMeasure{ cpuMeasure },
        m_netMeasure{ netMeasure } { }

    ~TimeWidget() noexcept = default;
    TimeWidget(const TimeWidget&) = delete;
    TimeWidget& operator=(const TimeWidget&) = delete;
    TimeWidget(TimeWidget&&) = delete;
    TimeWidget& operator=(TimeWidget&&) = delete;

    void draw() const override;
private:
    std::shared_ptr<CPUMeasure const> m_cpuMeasure{ nullptr };
    std::shared_ptr<NetMeasure const> m_netMeasure{ nullptr };
};

} // namespace rg
