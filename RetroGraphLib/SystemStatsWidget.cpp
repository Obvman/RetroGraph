#include "stdafx.h"

#include "SystemStatsWidget.h"

// #include <GL/glew.h>
// #include <GL/gl.h>
#include <Windows.h>

#include "colors.h"
#include "FontManager.h"
#include "SystemMeasure.h"
#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "DisplayMeasure.h"
#include "Monitors.h"
#include "RetroGraph.h"

namespace rg {

SystemStatsWidget::SystemStatsWidget(const FontManager* fontManager,
                                     const RetroGraph& rg, bool visible)
    : Widget{ fontManager, visible }
    , m_gpuMeasure{ &rg.getGPUMeasure() } {

    const auto& sysInfo{ rg.getSystemMeasure() };

    // Just create stats string here since we expect it not to change during
    // the lifetime of the program
    m_statsStrings.emplace_back(sysInfo.getUserName() + "@" +
                                sysInfo.getComputerName());
    m_statsStrings.emplace_back(sysInfo.getOSInfoStr());

    const auto& cpuMeasure{ rg.getCPUMeasure() };
    if (cpuMeasure.getCoreTempInfoSuccess())
        m_statsStrings.emplace_back(cpuMeasure.getCPUName());
    else
        m_statsStrings.emplace_back(sysInfo.getCPUDescription());

    m_statsStrings.emplace_back(m_gpuMeasure->getGpuDescription());
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

void SystemStatsWidget::updateObservers(const RetroGraph& rg) {
    m_gpuMeasure = &rg.getGPUMeasure();
}

void SystemStatsWidget::draw() const {
    if (!isVisible()) return;

    clear();

    drawWidgetBackground();

    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    glLineWidth(0.5f);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLines(RG_FONT_STANDARD, m_statsStrings, 0, 0,
                               m_viewport.width, m_viewport.height,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 10);
}

}
