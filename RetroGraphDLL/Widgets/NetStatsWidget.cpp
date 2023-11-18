module Widgets.NetStatsWidget;

import Colors;

import "GLHeaderUnit.h";

namespace rg {

NetStatsWidget::NetStatsWidget(const FontManager* fontManager, const IRetroGraph& rg, bool visible)
    : Widget{ fontManager, visible }
    , m_netMeasure{ &rg.getNetMeasure() } {

    m_statsStrings.emplace_back("Hostname: " + m_netMeasure->getHostname());
    m_statsStrings.emplace_back("LAN IP: " + m_netMeasure->getAdapterIP());
    m_statsStrings.emplace_back("DNS: " + m_netMeasure->getDNS());
    m_statsStrings.emplace_back("MAC: " + m_netMeasure->getAdapterMAC());
}

void NetStatsWidget::draw() const {
    std::vector<std::string_view> statsStrings{};
    for (const auto& str : m_statsStrings)
        statsStrings.push_back(str);

    const auto state = std::string{ "Connection Status: " } + (m_netMeasure->isConnected() ? "Up" : "Down");
    statsStrings.push_back(state);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLines(RG_FONT_STANDARD, statsStrings, 0, 0, m_viewport.width, m_viewport.height,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 10);
}

} // namespace rg
