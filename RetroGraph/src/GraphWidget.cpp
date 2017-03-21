#include "../headers/GraphWidget.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/CPUMeasure.h"
#include "../headers/RAMMeasure.h"
#include "../headers/NetMeasure.h"
#include "../headers/GPUMeasure.h"

namespace rg {

GraphWidget::GraphWidget() {

}

GraphWidget::~GraphWidget() {

}

void GraphWidget::init(const FontManager* fontManager, const CPUMeasure* cpuMeasure,
                       const RAMMeasure* ramMeasure, const NetMeasure* netMeasure,
                       const GPUMeasure* gpuMeasure, Viewport viewport) {

    m_fontManager = fontManager;
    m_cpuMeasure = cpuMeasure;
    m_ramMeasure = ramMeasure;
    m_netMeasure = netMeasure;
    m_gpuMeasure = gpuMeasure;

    m_cpuGraphVP = Viewport{
                     viewport.x,
                     viewport.y + 3*viewport.height/4,
                     viewport.width,
                     viewport.height/4 };
    m_ramGraphVP = Viewport{
                     viewport.x,
                     viewport.y + 0*viewport.height/4,
                     viewport.width,
                     viewport.height/4 };
    m_netGraphVP = Viewport{
                     viewport.x,
                     viewport.y + 1*viewport.height/4,
                     viewport.width,
                     viewport.height/4 };
    m_gpuGraphVP = Viewport{
                     viewport.x,
                     viewport.y + 2*viewport.height/4,
                     viewport.width,
                     viewport.height/4 };
}

void GraphWidget::draw() const {
    drawCpuGraph();
    drawRamGraph();
    drawNetGraph();
    drawGpuGraph();
}

void GraphWidget::drawCpuGraph() const {
    glViewport(m_cpuGraphVP.x, m_cpuGraphVP.y,
               m_cpuGraphVP.width, m_cpuGraphVP.height);
    drawBorder();

    // Set the viewport for the graph to be left section
    glViewport(m_cpuGraphVP.x, m_cpuGraphVP.y,
               (m_cpuGraphVP.width * 4)/5, m_cpuGraphVP.height);

    drawGraphGrid();
    glLineWidth(0.5f);
    drawLineGraph(m_cpuMeasure->getUsageData());

    // Text
    glViewport(m_cpuGraphVP.x + (4 * m_cpuGraphVP.width) / 5, m_cpuGraphVP.y,
               m_cpuGraphVP.width / 5, m_cpuGraphVP.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%",
                             0, 0,
                             m_cpuGraphVP.width/5, m_cpuGraphVP.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "CPU Load",
                             0, 0,
                             m_cpuGraphVP.width/5, m_cpuGraphVP.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%",
                             0, 0,
                             m_cpuGraphVP.width/5, m_cpuGraphVP.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);

}

void GraphWidget::drawRamGraph() const {
    glViewport(m_ramGraphVP.x, m_ramGraphVP.y,
               m_ramGraphVP.width , m_ramGraphVP.height);
    drawBorder();

    // Set the viewport for the graph itself to be left section
    glViewport(m_ramGraphVP.x, m_ramGraphVP.y,
               (m_ramGraphVP.width*4)/5 , m_ramGraphVP.height);

    // Draw the background grid for the graph
    drawGraphGrid();

    drawLineGraph(m_ramMeasure->getUsageData());

    // Set viewport for text drawing
    glViewport(m_ramGraphVP.x + (4 * m_ramGraphVP.width) / 5, m_ramGraphVP.y,
               m_ramGraphVP.width / 5, m_ramGraphVP.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%",
                             0, 0,
                             m_ramGraphVP.width/5, m_ramGraphVP.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "RAM Load",
                             0, 0,
                             m_ramGraphVP.width/5, m_ramGraphVP.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%",
                             0, 0,
                             m_ramGraphVP.width/5, m_ramGraphVP.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);

}

void GraphWidget::drawNetGraph() const {
    glViewport(m_netGraphVP.x, m_netGraphVP.y,
               m_netGraphVP.width , m_netGraphVP.height);
    drawBorder();

    // Set the viewport for the graph to be left section
    glViewport(m_netGraphVP.x, m_netGraphVP.y,
               (m_netGraphVP.width*4)/5 , m_netGraphVP.height);
    drawGraphGrid();

    {// Draw the line graph
        glLineWidth(0.5f);
        const auto& downData{ m_netMeasure->getDownData() };
        const auto& upData{ m_netMeasure->getUpData() };
        const auto maxDownValMB{ m_netMeasure->getMaxDownValue() /
                                 static_cast<float>(MB) };
        const auto maxUpValMB{ m_netMeasure->getMaxUpValue() /
                               static_cast<float>(MB) };

        const auto maxValMB{ max(maxUpValMB, maxDownValMB) };

        // Draw the download graph
        glBegin(GL_LINE_STRIP); {
            glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f);
            for (auto i = size_t{ 0U }; i < downData.size() - 1; ++i) {
                const auto percent1 = float{ (downData[i] / static_cast<float>(MB)) / maxValMB };
                const auto percent2 = float{ (downData[i+1] / static_cast<float>(MB)) / maxValMB };

                const auto x1 = float{ (static_cast<float>(i) / (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };
                const auto x2 = float{ (static_cast<float>(i+1) / (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, y1);    // Top-left
                glVertex2f(x2, y2);    // Top-right
            }
        } glEnd();

        // Draw the upload graph
        glBegin(GL_LINE_STRIP); {
            glColor4f(PINK1_R, PINK1_G, PINK1_B, 0.7f);
            for (auto i = size_t{ 0U }; i < upData.size() - 1; ++i) {
                const auto percent1 = float{ (upData[i] / static_cast<float>(MB)) / maxValMB };
                const auto percent2 = float{ (upData[i+1] / static_cast<float>(MB)) / maxValMB };

                const auto x1 = float{ (static_cast<float>(i) / (upData.size() - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };
                const auto x2 = float{ (static_cast<float>(i+1) / (upData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, y1); // Top-left
                glVertex2f(x2, y2); // Top-right
            }
        } glEnd();
    }

    {// Text
        glViewport(m_netGraphVP.x + (4 * m_netGraphVP.width) / 5, m_netGraphVP.y,
                   m_netGraphVP.width / 5, m_netGraphVP.height);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const auto maxVal{ m_netMeasure->getMaxDownValue() };
        std::string suffix{ "B" };
        if (maxVal > 1000 * 1000) {
            suffix = "MB";
        } else if (maxVal > 1000) {
            suffix = "KB";
        }

        /* Print the maximum throughput as the scale at the top of the graph */
        if (suffix == "MB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fMB", maxVal/static_cast<float>(MB));
            m_fontManager->renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        } else if (suffix == "KB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fKB", maxVal/static_cast<float>(KB));
            m_fontManager->renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        } else {
            const auto top{ std::to_string(maxVal) + suffix };
            char buff[5];
            snprintf(buff, sizeof(buff), "%3lluB", maxVal);
            m_fontManager->renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        }

        const auto bottom = std::string{ "0" + suffix };
        m_fontManager->renderLine(RG_FONT_SMALL, bottom.c_str(), 0, 0,
                                 m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                 RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
        m_fontManager->renderLine(RG_FONT_SMALL, "Down / Up", 0, 0,
                                 m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    }
}

void GraphWidget::drawGpuGraph() const {
    glViewport(m_gpuGraphVP.x, m_gpuGraphVP.y,
               m_gpuGraphVP.width , m_gpuGraphVP.height);
    drawBorder();

    // Set the viewport for the graph to be left section
    glViewport(m_gpuGraphVP.x, m_gpuGraphVP.y,
               (m_gpuGraphVP.width*4)/5 , m_gpuGraphVP.height);
    drawGraphGrid();
    glLineWidth(0.5f);
    drawLineGraph(m_gpuMeasure->getUsageData());

    // Set viewport for text drawing
    glViewport(m_gpuGraphVP.x + (4 * m_gpuGraphVP.width) / 5, m_gpuGraphVP.y,
               m_gpuGraphVP.width / 5, m_gpuGraphVP.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%",
                             0, 0,
                             m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "GPU Load",
                             0, 0,
                             m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%",
                             0, 0,
                             m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

}
