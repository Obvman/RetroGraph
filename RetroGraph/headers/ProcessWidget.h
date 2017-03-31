#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class ProcessMeasure;

class ProcessWidget : public Widget {
public:
    ProcessWidget();
    virtual ~ProcessWidget();
    ProcessWidget(const ProcessWidget&) = delete;
    ProcessWidget& operator=(const ProcessWidget&) = delete;

    virtual void draw() const override;

    void init(const FontManager* fontManager, const ProcessMeasure* procMeasure);

    void setViewport(Viewport vp) { m_viewport = vp; };
private:
    void drawCpuUsageList() const;
    void drawRamUsageList() const;

    const FontManager* m_fontManager;
    Viewport m_viewport;

    const ProcessMeasure* m_procMeasure;
};

}
