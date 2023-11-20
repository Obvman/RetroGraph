export module Widgets.ProcessRAMWidget;

import Measures.ProcessMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class ProcessRAMWidget : public Widget {
public:
    ProcessRAMWidget(const FontManager* fontManager, std::shared_ptr<const ProcessMeasure> processMeasure) :
        Widget{ fontManager },
        m_procMeasure{ processMeasure } {}

    ~ProcessRAMWidget() noexcept = default;

    void draw() const override;
private:
    std::shared_ptr<const ProcessMeasure> m_procMeasure{ nullptr };
};

} // namespace rg
