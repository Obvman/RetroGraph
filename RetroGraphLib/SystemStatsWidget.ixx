module;

export module SystemStatsWidget;

import Colors;
import CPUMeasure;
import DisplayMeasure;
import FontManager;
import GLListContainer;
import GPUMeasure;
import IRetroGraph;
import Monitors;
import SystemMeasure;
import Widget;

namespace rg {

export class SystemStatsWidget;

class SystemStatsWidget : public Widget {
public:
    SystemStatsWidget(const FontManager* fontManager,
                      const IRetroGraph& rg, bool visible);
    ~SystemStatsWidget() noexcept = default;

    void draw() const override;
private:
    std::vector<std::string> m_statsStrings{ };
};

SystemStatsWidget::SystemStatsWidget(const FontManager* fontManager,
                                     const IRetroGraph& rg, bool visible)
    : Widget{ fontManager, visible } {

    const auto& sysInfo{ rg.getSystemMeasure() };
    const auto& gpuMeasure{ rg.getGPUMeasure() };

    // Just create stats string here since we expect it not to change during
    // the lifetime of the program
    m_statsStrings.emplace_back(std::string{ sysInfo.getUserName() } +"@" + std::string{ sysInfo.getComputerName() });
    m_statsStrings.emplace_back(sysInfo.getOSInfoStr());

    const auto& cpuMeasure{ rg.getCPUMeasure() };
    if (cpuMeasure.getCoreTempInfoSuccess())
        m_statsStrings.emplace_back(cpuMeasure.getCPUName());
    else
        m_statsStrings.emplace_back(sysInfo.getCPUDescription());

    m_statsStrings.emplace_back(gpuMeasure.getGpuDescription());
    m_statsStrings.emplace_back(sysInfo.getRAMDescription());

    // Monitor information
    const auto& displayMeasure{ rg.getDisplayMeasure() };
    const auto& monitors{ displayMeasure.getMonitors() };

    if (monitors->getNumMonitors() > 1) {
        for (int i = 0; i < monitors->getNumMonitors() && i <= 3; ++i) {
            const auto& monitorData{ monitors->getMonitorData()[i] };

            std::ostringstream iss;
            iss << "Display " << i << ": " << monitorData.realWidth << "x" << monitorData.realHeight
                << "@" << monitorData.refreshRate << "Hz";

            m_statsStrings.emplace_back(iss.str());
        }
    } else {
        std::ostringstream iss;
        iss << "Display: ";
        const auto& monitorData{ monitors->getMonitorData()[0] };
        iss << monitorData.realWidth << "x" << monitorData.realHeight
            << "@" << monitorData.refreshRate << "Hz";
        m_statsStrings.emplace_back(iss.str());
    }
}

void SystemStatsWidget::draw() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLines(RG_FONT_STANDARD, m_statsStrings, 0, 0,
                               m_viewport.width, m_viewport.height,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 10);
}

}
