module Widgets.GraphWidget;

import Colors;
import Units;

namespace rg {

GraphWidget::GraphWidget(const FontManager* fontManager, size_t numGraphSamples)
    : Widget{ fontManager }
    , m_graph{ numGraphSamples } {

}

GPUGraphWidget::GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure)
    : GraphWidget{ fontManager, gpuMeasure->getUsageData().size() }
    , m_gpuMeasure{ gpuMeasure } {

}

void GPUGraphWidget::draw() const {
    if (m_gpuMeasure->isEnabled()) {
        // Set the viewport for the graph to be left section
        glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4) / 5, m_viewport.height);
        m_graph.updatePoints(m_gpuMeasure->getUsageData());
        m_graph.draw();
    }

    // Set viewport for text drawing
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y, m_viewport.width / 5, m_viewport.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "GPU Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}


CPUGraphWidget::CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure)
    : GraphWidget{ fontManager, cpuMeasure->getUsageData().size() }
    , m_cpuMeasure{ cpuMeasure } {

}

void CPUGraphWidget::draw() const {
    // Set the viewport for the graph to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4)/5, m_viewport.height);
    m_graph.updatePoints(m_cpuMeasure->getUsageData());
    m_graph.draw();

    // Text
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y, m_viewport.width / 5, m_viewport.height);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "CPU Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

RAMGraphWidget::RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure)
    : GraphWidget{ fontManager, ramMeasure->getUsageData().size() }
    , m_ramMeasure{ ramMeasure } {

}

void RAMGraphWidget::draw() const {
    // Set the viewport for the graph itself to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width*4)/5 , m_viewport.height);
    m_graph.updatePoints(m_ramMeasure->getUsageData());
    m_graph.draw();

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
    : GraphWidget{ fontManager, netMeasure->getDownData().size() }
    , m_netMeasure{ netMeasure }
    , m_netUpGraph{ m_netMeasure->getUpData().size(), { PINK1_R, PINK1_G, PINK1_B, 0.7f }, false } {

    m_graph.setColor({ GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f });
}

void NetGraphWidget::draw() const {
    // Set the viewport for the graph to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4) / 5, m_viewport.height);

    {// Draw the line graphs
        const auto& downData{ m_netMeasure->getDownData() };
        const auto& upData{ m_netMeasure->getUpData() };
        const auto maxDownValMB{ m_netMeasure->getMaxDownValue() / static_cast<float>(MB) };
        const auto maxUpValMB{ m_netMeasure->getMaxUpValue() / static_cast<float>(MB) };

        const auto maxValMB{ std::max(maxUpValMB, maxDownValMB) };

        std::vector<float> normalizedDownData(downData.size());
        for (auto i = int{ 0 }; i < downData.size(); ++i) {
            normalizedDownData[i] = (downData[i] / static_cast<float>(MB)) / maxValMB;
        }
        std::vector<float> normalizedUpData(upData.size());
        for (auto i = int{ 0 }; i < upData.size(); ++i) {
            normalizedUpData[i] = (upData[i] / static_cast<float>(MB)) / maxValMB;
        }
        m_netUpGraph.updatePoints(normalizedUpData);
        m_graph.updatePoints(normalizedDownData);

        m_netUpGraph.draw();
        m_graph.draw();
    }

    {// Text
        glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y, m_viewport.width / 5, m_viewport.height);
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
