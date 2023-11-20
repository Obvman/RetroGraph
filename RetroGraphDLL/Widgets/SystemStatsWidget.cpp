module Widgets.SystemStatsWidget;

import Colors;

namespace rg {

SystemStatsWidget::SystemStatsWidget(const FontManager* fontManager,
                                     std::shared_ptr<CPUMeasure const> cpuMeasure,
                                     std::shared_ptr<GPUMeasure const> gpuMeasure,
                                     std::shared_ptr<DisplayMeasure const> displayMeasure,
                                     std::shared_ptr<SystemMeasure const> systemMeasure,
                                     bool visible)
    : Widget{ fontManager, visible } {

    // Just create stats string here since we expect it not to change during
    // the lifetime of the program
    m_statsStrings.emplace_back(std::string{ systemMeasure->getUserName() } +"@" + std::string{ systemMeasure->getComputerName() });
    m_statsStrings.emplace_back(systemMeasure->getOSInfoStr());

    if (cpuMeasure->getCoreTempInfoSuccess()) {
        const auto cpuName = cpuMeasure->getCPUName();
        if (!cpuName.empty())
            m_statsStrings.emplace_back(cpuName);
    }
    else
        m_statsStrings.emplace_back(systemMeasure->getCPUDescription());

    m_statsStrings.emplace_back(gpuMeasure->getGpuDescription());
    m_statsStrings.emplace_back(systemMeasure->getRAMDescription());

    // Monitor information
    const auto& monitors{ displayMeasure->getMonitors() };

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

} // namespace rg
