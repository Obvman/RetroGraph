export module Widgets.ProcessCPUWidget;

import Measures.ProcessMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class ProcessCPUWidget : public Widget {
public:
    ProcessCPUWidget(const FontManager* fontManager, std::shared_ptr<const ProcessMeasure> processMeasure) :
        Widget{ fontManager },
        m_procMeasure{ processMeasure } {}

    ~ProcessCPUWidget() noexcept = default;

    void draw() const override;
private:
    std::shared_ptr<const ProcessMeasure> m_procMeasure{ nullptr };
};

} // namespace rg
