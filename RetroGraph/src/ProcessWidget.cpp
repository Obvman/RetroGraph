#include "../headers/ProcessWidget.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/ProcessMeasure.h"

namespace rg {

ProcessWidget::ProcessWidget() {

}

ProcessWidget::~ProcessWidget() {

}

void ProcessWidget::init(const FontManager* fontManager, const ProcessMeasure* procMeasure) {
    m_fontManager = fontManager;
    m_procMeasure = procMeasure;
}

void ProcessWidget::draw() const {
    glViewport(m_viewport.x, m_viewport.y,
               m_viewport.width, m_viewport.height);

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);
    glBegin(GL_LINES); {
        glVertex2f(0.0f, -1.0f);
        glVertex2f(0.0f,  1.0f); // Middle line
    } glEnd();

    drawCpuUsageList();
    drawRamUsageList();
}

void ProcessWidget::drawCpuUsageList() const {
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

    m_fontManager->renderLines(RG_FONT_STANDARD, procNames, 0, 0,
                              m_viewport.width/2, m_viewport.height,
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
    m_fontManager->renderLines(RG_FONT_STANDARD, procPercentages, 0, 0,
                              m_viewport.width/2, m_viewport.height,
                              RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
}

void ProcessWidget::drawRamUsageList() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    auto procNames = std::vector<std::string>{  };
    auto procRamUsages = std::vector<std::string>{  };

    procNames.reserve(m_procMeasure->getProcRAMData().size());
    procRamUsages.reserve(m_procMeasure->getProcRAMData().size());
    for (const auto& pair : m_procMeasure->getProcRAMData()) {
        procNames.emplace_back(pair.first);

        // Convert RAM value to string format, assuming top RAM usages are only
        // ever in megabytes or gigabytes
        char buff[6];
        if (pair.second >= 1000) {
            snprintf(buff, sizeof(buff), "%.1fGB", pair.second / 1024.0f);
        } else {
            snprintf(buff, sizeof(buff), "%dMB", pair.second);
        }
        procRamUsages.emplace_back(buff);
    }

    m_fontManager->renderLines(RG_FONT_STANDARD, procNames, m_viewport.width/2, 0,
                              m_viewport.width/2, m_viewport.height,
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
    m_fontManager->renderLines(RG_FONT_STANDARD, procRamUsages, m_viewport.width/2, 0,
                              m_viewport.width/2, m_viewport.height,
                              RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);


}

}
