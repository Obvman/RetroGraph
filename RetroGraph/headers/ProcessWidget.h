#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class ProcessMeasure;

class ProcessWidget {
public:
    ProcessWidget(const FontManager* fontManager,
                  const ProcessMeasure* procMeasure,
                  bool visible) :
        m_fontManager{ fontManager }, m_procMeasure{ procMeasure },
        m_visible{ visible } {}

    ~ProcessWidget() noexcept = default;
    ProcessWidget(const ProcessWidget&) = delete;
    ProcessWidget& operator=(const ProcessWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };

    void setVisible(bool b) { m_visible = b; }
private:
    void drawCpuUsageList() const;
    void drawRamUsageList() const;

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    bool m_visible{ true };

    const ProcessMeasure* m_procMeasure{ nullptr };
};

}
