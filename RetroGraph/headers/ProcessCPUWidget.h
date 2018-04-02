#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class ProcessMeasure;

class ProcessCPUWidget {
public:
    ProcessCPUWidget(const FontManager* fontManager,
                     const ProcessMeasure* procMeasure,
                     bool visible) :
        m_fontManager{ fontManager },  m_visible{ visible }, 
        m_procMeasure{ procMeasure } {}

    ~ProcessCPUWidget() noexcept = default;
    ProcessCPUWidget(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget& operator=(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget(ProcessCPUWidget&&) = delete;
    ProcessCPUWidget& operator=(ProcessCPUWidget&&) = delete;

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

} // namespace rg
