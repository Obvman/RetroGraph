#pragma once

#include "stdafx.h"

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class CPUMeasure;
class NetMeasure;

class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               const std::unique_ptr<CPUMeasure>& cpuMeasure,
               const std::unique_ptr<NetMeasure>& netMeasure,
               bool visible) :
        Widget{ fontManager, visible },
        m_cpuMeasure{ cpuMeasure.get() },
        m_netMeasure{ netMeasure.get() } { }

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
