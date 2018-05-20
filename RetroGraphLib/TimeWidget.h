#pragma once

#include "stdafx.h"

#include "Widget.h"
#include "RetroGraph.h"

namespace rg {

class CPUMeasure;
class NetMeasure;

class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               const RetroGraph& rg, bool visible) :
        Widget{ fontManager, visible },
        m_cpuMeasure{ rg.getCPUMeasure().get() },
        m_netMeasure{ rg.getNetMeasure().get() } { }

    ~TimeWidget() noexcept = default;
    TimeWidget(const TimeWidget&) = delete;
    TimeWidget& operator=(const TimeWidget&) = delete;
    TimeWidget(TimeWidget&&) = delete;
    TimeWidget& operator=(TimeWidget&&) = delete;

    void updateObservers(const RetroGraph& rg) override;

    void draw() const override;
private:
    const CPUMeasure* m_cpuMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
};

} // namespace rg
