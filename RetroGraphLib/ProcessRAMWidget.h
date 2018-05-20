#pragma once

#include "stdafx.h"

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;
class ProcessMeasure;

class ProcessRAMWidget : public Widget{
public:
    ProcessRAMWidget(const FontManager* fontManager,
                     const std::unique_ptr<ProcessMeasure>& procMeasure,
                     bool visible) :
        Widget{ fontManager, visible },
        m_procMeasure{ procMeasure.get() } {}

    ~ProcessRAMWidget() noexcept = default;
    ProcessRAMWidget(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget& operator=(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget(ProcessRAMWidget&&) = delete;
    ProcessRAMWidget& operator=(ProcessRAMWidget&&) = delete;

    void updateObservers(const RetroGraph& rg) override;
    void draw() const override;
private:
    const ProcessMeasure* m_procMeasure{ nullptr };
};

} // namespace rg
