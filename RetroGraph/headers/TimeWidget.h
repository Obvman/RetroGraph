#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;
class NetMeasure;

class TimeWidget : public Widget {
public:
    TimeWidget() = default;
    virtual ~TimeWidget() noexcept = default;
    TimeWidget(const TimeWidget&) = delete;
    TimeWidget& operator=(const TimeWidget&) = delete;

    void draw() const override;

    void init(const FontManager* fontManager,
              const CPUMeasure* cpuMeasure,
              const NetMeasure* netMeasure);

    void setViewport(Viewport vp) { m_viewport = vp; };
private:
    const FontManager* m_fontManager{ nullptr };

    Viewport m_viewport{ };
    const CPUMeasure* m_cpuMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
};

}
