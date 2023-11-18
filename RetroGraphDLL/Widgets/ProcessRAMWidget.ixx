export module Widgets.ProcessRAMWidget;

import IRetroGraph; // Reverse Dependency

import Measures.ProcessMeasure;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class ProcessRAMWidget : public Widget {
public:
    ProcessRAMWidget(const FontManager* fontManager,
                     const IRetroGraph& rg,
                     bool visible) :
        Widget{ fontManager, visible },
        m_procMeasure{ &rg.getProcessMeasure() } {}

    ~ProcessRAMWidget() noexcept = default;
    ProcessRAMWidget(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget& operator=(const ProcessRAMWidget&) = delete;
    ProcessRAMWidget(ProcessRAMWidget&&) = delete;
    ProcessRAMWidget& operator=(ProcessRAMWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;
private:
    const ProcessMeasure* m_procMeasure{ nullptr };
};

} // namespace rg
