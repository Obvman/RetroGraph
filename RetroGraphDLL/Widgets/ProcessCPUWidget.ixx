export module RG.Widgets:ProcessCPUWidget;

import :Widget;

import RG.Measures;
import RG.Rendering;

import std;

namespace rg {

export class ProcessCPUWidget : public Widget {
public:
    ProcessCPUWidget(const FontManager* fontManager, std::shared_ptr<const ProcessMeasure> processMeasure);
    ~ProcessCPUWidget() noexcept;

    void draw() const override;

private:
    PostUpdateEvent::Handle RegisterPostUpdateCallback();

    std::shared_ptr<const ProcessMeasure> m_procMeasure{ nullptr };
    PostUpdateEvent::Handle m_postUpdateHandle;
};

} // namespace rg
