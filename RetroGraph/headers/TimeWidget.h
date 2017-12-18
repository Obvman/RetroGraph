#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;
class NetMeasure;

class TimeWidget {
public:
    TimeWidget(const FontManager* fontManager,
               const CPUMeasure* cpuMeasure,
               const NetMeasure* netMeasure,
               bool visible) :
        m_fontManager{ fontManager },
        m_visible{ visible },
        m_cpuMeasure{ cpuMeasure },
        m_netMeasure{ netMeasure } { }

    ~TimeWidget() noexcept = default;
    TimeWidget(const TimeWidget&) = delete;
    TimeWidget& operator=(const TimeWidget&) = delete;

    void draw() const;
    void clear() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void setVisibility(bool b);
private:
    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };

    const CPUMeasure* m_cpuMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
};

}
