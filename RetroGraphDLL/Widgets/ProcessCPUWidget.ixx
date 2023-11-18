export module Widgets.ProcessCPUWidget;

import IRetroGraph; // Reverse Dependency

import Measures.ProcessMeasure;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class ProcessCPUWidget : public Widget {
public:
    ProcessCPUWidget(const FontManager* fontManager,
                     const IRetroGraph& rg,
                     bool visible) :
        Widget{ fontManager, visible },
        m_procMeasure{ &rg.getProcessMeasure() } {}

    ~ProcessCPUWidget() noexcept = default;
    ProcessCPUWidget(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget& operator=(const ProcessCPUWidget&) = delete;
    ProcessCPUWidget(ProcessCPUWidget&&) = delete;
    ProcessCPUWidget& operator=(ProcessCPUWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;
private:
    const ProcessMeasure* m_procMeasure{ nullptr };
};

} // namespace rg
