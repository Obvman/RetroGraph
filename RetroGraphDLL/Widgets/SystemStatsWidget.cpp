module RG.Widgets:SystemStatsWidget;

import Colors;

import RG.Core;

namespace rg {

SystemStatsWidget::SystemStatsWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure,
                                     std::shared_ptr<const GPUMeasure> gpuMeasure,
                                     std::shared_ptr<const RAMMeasure> ramMeasure,
                                     std::shared_ptr<const DisplayMeasure> displayMeasure,
                                     std::shared_ptr<const SystemMeasure> systemMeasure)
    : Widget{ fontManager } {
    // Just create stats string here since we expect it not to change during
    // the lifetime of the program
    m_statsStrings.emplace_back("User Name: " + systemMeasure->getUserName() + "@" + systemMeasure->getComputerName());
    m_statsStrings.emplace_back("OS: " + systemMeasure->getOSName());
    m_statsStrings.emplace_back("OS Version: " + systemMeasure->getOSVersion());
    m_statsStrings.emplace_back("CPU: " + cpuMeasure->getCPUName());
    m_statsStrings.emplace_back(std::format("GPU: {} ({})", gpuMeasure->getGPUName(), gpuMeasure->getDriverVersion()));
    m_statsStrings.emplace_back(std::format("RAM: {:2.1f}GB", ramMeasure->getRAMCapacity() / static_cast<float>(GB)));

    // Monitor information
    const auto& monitors{ displayMeasure->getMonitors() };

    if (monitors->getNumMonitors() > 1) {
        for (int i = 0; i < monitors->getNumMonitors() && i <= 3; ++i) {
            const auto& monitorData{ monitors->getMonitorData()[i] };

            std::ostringstream iss;
            iss << "Display " << i << ": " << monitorData.realWidth << "x" << monitorData.realHeight << "@"
                << monitorData.refreshRate << "Hz";

            m_statsStrings.emplace_back(iss.str());
        }
    } else {
        std::ostringstream iss;
        iss << "Display: ";
        const auto& monitorData{ monitors->getMonitorData()[0] };
        iss << monitorData.realWidth << "x" << monitorData.realHeight << "@" << monitorData.refreshRate << "Hz";
        m_statsStrings.emplace_back(iss.str());
    }
}

void SystemStatsWidget::draw() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLines(RG_FONT_STANDARD, m_statsStrings, 0, 0, m_viewport.width, m_viewport.height,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 10);
}

} // namespace rg
