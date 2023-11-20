export module Widgets.ProcessCPUWidget;

import Measures.ProcessMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class ProcessCPUWidget : public Widget {
public:
    ProcessCPUWidget(const FontManager* fontManager,
                     std::shared_ptr<ProcessMeasure const> processMeasure,
                     bool visible) :
        Widget{ fontManager, visible },
        m_procMeasure{ processMeasure } {}

    ~ProcessCPUWidget() noexcept = default;
    ProcessCPUWidget(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget& operator=(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget(ProcessCPUWidget&&) = delete;
    ProcessCPUWidget& operator=(ProcessCPUWidget&&) = delete;

    void draw() const override;
private:
    std::shared_ptr<ProcessMeasure const> m_procMeasure{ nullptr };
};

} // namespace rg
