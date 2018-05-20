#pragma once

#include "stdafx.h"

#include "drawUtils.h"
#include "Widget.h"
#include "RetroGraph.h"

namespace rg {

class ProcessMeasure;

class ProcessCPUWidget : public Widget {
public:
    ProcessCPUWidget(const FontManager* fontManager,
                     const RetroGraph& rg,
                     bool visible) :
        Widget{ fontManager, visible }, 
        m_procMeasure{ rg.getProcessMeasure().get() } {}

    ~ProcessCPUWidget() noexcept = default;
    ProcessCPUWidget(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget& operator=(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget(ProcessCPUWidget&&) = delete;
    ProcessCPUWidget& operator=(ProcessCPUWidget&&) = delete;

    void updateObservers(const RetroGraph& rg) override;
    void draw() const override;
private:
    const ProcessMeasure* m_procMeasure{ nullptr };
};

} // namespace rg
