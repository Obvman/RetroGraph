export module Widgets.SystemStatsWidget;

import Measures.CPUMeasure;
import Measures.DisplayMeasure;
import Measures.GPUMeasure;
import Measures.SystemMeasure;

import Rendering.FontManager;

import Widgets.Widget;

import std.core;
import std.memory;

namespace rg {

export class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager,
                      std::shared_ptr<CPUMeasure const> cpuMeasure,
                      std::shared_ptr<GPUMeasure const> gpuMeasure,
                      std::shared_ptr<DisplayMeasure const> displayMeasure,
                      std::shared_ptr<SystemMeasure const> systemMeasure,
                      bool visible);
    ~SystemStatsWidget() noexcept = default;

    void draw() const override;
private:
    std::vector<std::string> m_statsStrings{ };
};

} // namespace rg
