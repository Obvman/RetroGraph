#pragma once

#include "drawUtils.h"
#include "Widget.h"

namespace rg {

class FontManager;
class ProcessMeasure;

class ProcessRAMWidget : public Widget{
public:
    ProcessRAMWidget(const FontManager* fontManager,
                     const ProcessMeasure* procMeasure,
                     bool visible) :
        Widget{ visible },
        m_fontManager{ fontManager },
        m_procMeasure{ procMeasure } {}

    ~ProcessRAMWidget() noexcept = default;
    ProcessRAMWidget(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget& operator=(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget(ProcessRAMWidget&&) = delete;
    ProcessRAMWidget& operator=(ProcessRAMWidget&&) = delete;

    void draw() const override;
private:
    const FontManager* m_fontManager{ nullptr };
    const ProcessMeasure* m_procMeasure{ nullptr };
};

} // namespace rg
