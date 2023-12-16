export module Widgets.SystemStatsWidget;

import RG.Measures;
import RG.Rendering;

import Widgets.Widget;

import std.core;
import std.memory;

namespace rg {

export class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager,
                      std::shared_ptr<const CPUMeasure> cpuMeasure,
                      std::shared_ptr<const GPUMeasure> gpuMeasure,
                      std::shared_ptr<const DisplayMeasure> displayMeasure,
                      std::shared_ptr<const SystemMeasure> systemMeasure);
    ~SystemStatsWidget() noexcept = default;

    void draw() const override;
private:
    std::vector<std::string> m_statsStrings{ };
};

} // namespace rg
