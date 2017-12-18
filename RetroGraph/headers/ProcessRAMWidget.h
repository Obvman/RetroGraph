#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class ProcessMeasure;

class ProcessRAMWidget {
public:
    ProcessRAMWidget(const FontManager* fontManager,
                     const ProcessMeasure* procMeasure,
                     bool visible) :
        m_fontManager{ fontManager }, m_procMeasure{ procMeasure },
        m_visible{ visible } {}

    ~ProcessRAMWidget() noexcept = default;
    ProcessRAMWidget(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget& operator=(const ProcessRAMWidget&) = delete;

    void draw() const;
    void clear() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void setVisibility(bool b);

private:

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };

    const ProcessMeasure* m_procMeasure{ nullptr };
};

}
