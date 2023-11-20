export module Widgets.ProcessRAMWidget;

import Measures.ProcessMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class ProcessRAMWidget : public Widget {
public:
    ProcessRAMWidget(const FontManager* fontManager,
                     std::shared_ptr<ProcessMeasure const> processMeasure,
                     bool visible) :
        Widget{ fontManager, visible },
        m_procMeasure{ processMeasure } {}

    ~ProcessRAMWidget() noexcept = default;
    ProcessRAMWidget(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget& operator=(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget(ProcessRAMWidget&&) = delete;
    ProcessRAMWidget& operator=(ProcessRAMWidget&&) = delete;

    void draw() const override;
private:
    std::shared_ptr<ProcessMeasure const> m_procMeasure{ nullptr };
};

} // namespace rg
