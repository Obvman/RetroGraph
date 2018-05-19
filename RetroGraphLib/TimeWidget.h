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
               const CPUMeasure* cpuMeasure,
               const NetMeasure* netMeasure,
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
    const CPUMeasure* m_cpuMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
};

} // namespace rg
