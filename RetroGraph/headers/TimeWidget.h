#pragma once

#include "Widget.h"

#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;
class NetMeasure;

class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               const CPUMeasure* cpuMeasure,
               const NetMeasure* netMeasure) :
        m_fontManager{ fontManager },
        m_cpuMeasure{ cpuMeasure },
        m_netMeasure{ netMeasure } { }

    virtual ~TimeWidget() noexcept = default;
    TimeWidget(const TimeWidget&) = delete;
    TimeWidget& operator=(const TimeWidget&) = delete;

    void draw() const override;

    void setViewport(Viewport vp) { m_viewport = vp; };
private:
    const FontManager* m_fontManager{ nullptr };

    Viewport m_viewport{ };
    const CPUMeasure* m_cpuMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
};

}
