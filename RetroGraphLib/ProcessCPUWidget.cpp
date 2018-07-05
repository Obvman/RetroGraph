#include "stdafx.h"

#include "ProcessCPUWidget.h"

// #include <GL/glew.h>
// #include <GL/gl.h>
#include <Windows.h>

#include "colors.h"
#include "FontManager.h"
#include "ProcessMeasure.h"
#include "RetroGraph.h"
#include "ListContainer.h"

namespace rg {

void ProcessCPUWidget::updateObservers(const RetroGraph & rg) {
    m_procMeasure = &rg.getProcessMeasure();
}

void ProcessCPUWidget::draw() const {
    if (!isVisible()) return;

    clear();

    drawWidgetBackground();

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);
    ListContainer::inst().drawTopAndBottomSerifs();

    // Draw the list itself
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    auto procNames = std::vector<std::string>{  };
    auto procPercentages = std::vector<std::string>{  };
    procNames.reserve(m_procMeasure->getProcCPUData().size());
    procPercentages.reserve(m_procMeasure->getProcCPUData().size());
    for (const auto& pair : m_procMeasure->getProcCPUData()) {
        procNames.emplace_back(pair.first);

        // Convert percentage to string format
        char buff[6];
        snprintf(buff, sizeof(buff), "%4.1f%%", pair.second);
        procPercentages.emplace_back(buff);
    }

    m_fontManager->renderLines(RG_FONT_STANDARD, procNames, 0, 0, 0, 0,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 5);
    m_fontManager->renderLines(RG_FONT_STANDARD, procPercentages, 0, 0, 0, 0,
                               RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 5);
}

}
