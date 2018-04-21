#pragma once

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;
class CPUMeasure;
class NetMeasure;

class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               const CPUMeasure* cpuMeasure,
               const NetMeasure* netMeasure,
               bool visible) :
        Widget{ visible },
        m_fontManager{ fontManager },
        m_cpuMeasure{ cpuMeasure },
        m_netMeasure{ netMeasure } { }

    ~TimeWidget() noexcept = default;
    TimeWidget(const TimeWidget&) = delete;
    TimeWidget& operator=(const TimeWidget&) = delete;
    TimeWidget(TimeWidget&&) = delete;
    TimeWidget& operator=(TimeWidget&&) = delete;

    void draw() const override;
private:
    const FontManager* m_fontManager{ nullptr };

    const CPUMeasure* m_cpuMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
};

} // namespace rg
