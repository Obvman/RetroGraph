module Widgets.GraphWidget;

import Colors;
import Units;

import Rendering.GLListContainer;
import Rendering.VBOController;

import "GLHeaderUnit.h";

namespace rg {

GPUGraphWidget::GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure)
    : Widget(fontManager)
    , m_gpuMeasure(gpuMeasure)
    , m_gpuVBO{ VBOController::inst().createGraphLineVBO(m_gpuMeasure->getUsageData().size()) } {

}

void GPUGraphWidget::draw() const {
    // Set the viewport for the graph to be left section
    glViewport(m_viewport.x, m_viewport.y,
               (m_viewport.width*4)/5 , m_viewport.height);
    GLListContainer::inst().drawBorder();
    VBOController::inst().drawGraphGrid();

    if (m_gpuMeasure->isEnabled()) {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
        glLineWidth(0.5f);

        VBOController::inst().updateGraphLines(m_gpuVBO, m_gpuMeasure->getUsageData());
        VBOController::inst().drawGraphLines(m_gpuVBO);
    }

    // Set viewport for text drawing
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y,
               m_viewport.width / 5, m_viewport.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "GPU Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}


CPUGraphWidget::CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure)
    : Widget(fontManager)
    , m_cpuMeasure(cpuMeasure)
    , m_cpuVBO{ VBOController::inst().createGraphLineVBO(m_cpuMeasure->getUsageData().size()) } {

}

void CPUGraphWidget::draw() const {
    // Set the viewport for the graph to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4)/5, m_viewport.height);

    GLListContainer::inst().drawBorder();

    VBOController::inst().drawGraphGrid();

    glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    glLineWidth(0.5f);
    VBOController::inst().updateGraphLines(m_cpuVBO, m_cpuMeasure->getUsageData());
    VBOController::inst().drawGraphLines(m_cpuVBO);

    // Text
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y,
               m_viewport.width / 5, m_viewport.height);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "CPU Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

RAMGraphWidget::RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure)
    : Widget(fontManager)
    , m_ramMeasure(ramMeasure)
    , m_ramVBO{ VBOController::inst().createGraphLineVBO(m_ramMeasure->getUsageData().size()) } {

}

void RAMGraphWidget::draw() const {
    // Set the viewport for the graph itself to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width*4)/5 , m_viewport.height);

    GLListContainer::inst().drawBorder();

    // Draw the background grid for the graph
    VBOController::inst().drawGraphGrid();

    glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    glLineWidth(0.5f);
    VBOController::inst().updateGraphLines(m_ramVBO, m_ramMeasure->getUsageData());
    VBOController::inst().drawGraphLines(m_ramVBO);

    // Set viewport for text drawing
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y,
               m_viewport.width / 5, m_viewport.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "RAM Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);

}

NetGraphWidget::NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure)
    : Widget(fontManager)
    , m_netMeasure(netMeasure)
    , m_netUpVBO{ VBOController::inst().createGraphLineVBO(m_netMeasure->getUpData().size()) }
    , m_netDownVBO{ VBOController::inst().createGraphLineVBO(m_netMeasure->getDownData().size()) } {

}

void NetGraphWidget::draw() const {
    // Set the viewport for the graph to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4) / 5, m_viewport.height);
    VBOController::inst().drawGraphGrid();
    GLListContainer::inst().drawBorder();

    {// Draw the line graph
        glLineWidth(0.5f);
        const auto& downData{ m_netMeasure->getDownData() };
        const auto& upData{ m_netMeasure->getUpData() };
        const auto maxDownValMB{ m_netMeasure->getMaxDownValue() /
                                 static_cast<float>(MB) };
        const auto maxUpValMB{ m_netMeasure->getMaxUpValue() /
                               static_cast<float>(MB) };

        const auto maxValMB{ std::max(maxUpValMB, maxDownValMB) };

        // #TODO VBO these!
        // Draw the download graph
        glBegin(GL_LINE_STRIP);
        {
            glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f);
            for (auto i = size_t{ 0U }; i < downData.size() - 1; ++i) {
                const auto percent1 = float{ (downData[i] /
                                              static_cast<float>(MB)) /
                                              maxValMB };
                const auto percent2 = float{ (downData[i + 1] /
                                              static_cast<float>(MB)) /
                                              maxValMB };

                const auto x1 = float{ (static_cast<float>(i) /
                                        (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };
                const auto x2 = float{ (static_cast<float>(i + 1) /
                                        (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, y1);    // Top-left
                glVertex2f(x2, y2);    // Top-right
            }
        } glEnd();

        // Draw the upload graph
        glBegin(GL_LINE_STRIP);
        {
            glColor4f(PINK1_R, PINK1_G, PINK1_B, 0.7f);
            for (auto i = size_t{ 0U }; i < upData.size() - 1; ++i) {
                const auto percent1 = float{ (upData[i] /
                        static_cast<float>(MB)) / maxValMB };
                const auto percent2 = float{ (upData[i + 1] /
                        static_cast<float>(MB)) / maxValMB };

                const auto x1 = float{ (static_cast<float>(i) / (upData.size()
                            - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };

                const auto x2 = float{ (static_cast<float>(i + 1) /
                        (upData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, y1); // Top-left
                glVertex2f(x2, y2); // Top-right
            }
        } glEnd();
    }

    {// Text
        glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y,
                   m_viewport.width / 5, m_viewport.height);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const auto maxVal{ m_netMeasure->getMaxDownValue() };

        const std::string_view suffix = [maxVal]() {
            if (maxVal > 1000 * 1000)
                return "MB";
            else if (maxVal > 1000)
                return "KB";
            else
                return "B";
        }(); // Call immediately

        /* Print the maximum throughput as the scale at the top of the graph */
        if (suffix == "MB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fMB", maxVal / static_cast<float>(MB));
            m_fontManager->renderLine(RG_FONT_SMALL, buff, 0, 0, m_viewport.width / 5, m_viewport.height,
                                      RG_ALIGN_TOP | RG_ALIGN_LEFT);
        } else if (suffix == "KB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fKB", maxVal / static_cast<float>(KB));
            m_fontManager->renderLine(RG_FONT_SMALL, buff, 0, 0, m_viewport.width / 5, m_viewport.height,
                                      RG_ALIGN_TOP | RG_ALIGN_LEFT);
        } else {
            char buff[5];
            snprintf(buff, sizeof(buff), "%3lluB", maxVal);
            m_fontManager->renderLine(RG_FONT_SMALL, buff, 0, 0, m_viewport.width / 5, m_viewport.height,
                                      RG_ALIGN_TOP | RG_ALIGN_LEFT);
        }

        m_fontManager->renderLine(RG_FONT_SMALL, "Down / Up", 0, 0, m_viewport.width / 5, m_viewport.height,
                                  RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT);
        m_fontManager->renderLine(RG_FONT_SMALL, "0B", 0, 0, m_viewport.width / 5, m_viewport.height,
                                  RG_ALIGN_BOTTOM | RG_ALIGN_LEFT);
    }
}

} // namespace rg
