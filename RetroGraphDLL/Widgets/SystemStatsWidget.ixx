export module RG.Widgets:SystemStatsWidget;

import :Widget;

import RG.Measures;
import RG.Rendering;

import std;

namespace rg {

export class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure,
                      std::shared_ptr<const GPUMeasure> gpuMeasure, std::shared_ptr<const RAMMeasure> ramMeasure,
                      std::shared_ptr<const DisplayMeasure> displayMeasure,
                      std::shared_ptr<const SystemMeasure> systemMeasure);
    ~SystemStatsWidget() noexcept = default;

    void draw() const override;

private:
    std::vector<std::string> m_statsStrings{};
};

} // namespace rg
