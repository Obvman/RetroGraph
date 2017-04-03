#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class ProcessMeasure;

class ProcessWidget {
public:
    ProcessWidget(const FontManager* fontManager,
                  const ProcessMeasure* procMeasure) :
        m_fontManager{ fontManager }, m_procMeasure{ procMeasure } {}

    ~ProcessWidget() noexcept = default;
    ProcessWidget(const ProcessWidget&) = delete;
    ProcessWidget& operator=(const ProcessWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp) { m_viewport = vp; };
private:
    void drawCpuUsageList() const;
    void drawRamUsageList() const;

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };

    const ProcessMeasure* m_procMeasure{ nullptr };
};

}
