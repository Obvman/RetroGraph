export module Widgets.TimeWidget;

import Measures.CPUMeasure;
import Measures.NetMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.memory;

namespace rg {

export class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               std::shared_ptr<const CPUMeasure> cpuMeasure,
               std::shared_ptr<const NetMeasure> netMeasure) :
        Widget{ fontManager },
        m_cpuMeasure{ cpuMeasure },
        m_netMeasure{ netMeasure } { }

    ~TimeWidget() noexcept = default;

    void draw() const override;

    // TODO use invalidation
    bool needsRedraw() const { return true; } // draw every frame
private:
    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };
    std::shared_ptr<const NetMeasure> m_netMeasure{ nullptr };
};

} // namespace rg
