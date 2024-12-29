export module RG.Widgets:ProcessRAMWidget;

import :Widget;

import RG.Measures;
import RG.Rendering;

import std;

namespace rg {

export class ProcessRAMWidget : public Widget {
public:
    ProcessRAMWidget(const FontManager* fontManager, std::shared_ptr<const ProcessMeasure> processMeasure)
        : Widget{ fontManager }
        , m_procMeasure{ processMeasure } {}

    ~ProcessRAMWidget();

    void draw() const override;

private:
    PostUpdateEvent::Handle RegisterPostUpdateCallback();

    std::shared_ptr<const ProcessMeasure> m_procMeasure{ nullptr };
    PostUpdateEvent::Handle m_postUpdateHandle;
};

} // namespace rg
