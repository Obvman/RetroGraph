module RG.Widgets:NetStatsWidget;

import Colors;

import "GLHeaderUnit.h";

namespace rg {

NetStatsWidget::NetStatsWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure)
    : Widget{ fontManager }
    , m_netMeasure{ netMeasure }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() } {
    m_statsStrings.emplace_back("Hostname: " + m_netMeasure->getHostname());
    m_statsStrings.emplace_back("LAN IP: " + m_netMeasure->getAdapterIP());
    m_statsStrings.emplace_back("DNS: " + m_netMeasure->getDNS());
    m_statsStrings.emplace_back("MAC: " + m_netMeasure->getAdapterMAC());
    m_statsStrings.emplace_back(std::string{ "Connection Status: " } + (m_netMeasure->isConnected() ? "Up" : "Down"));
}

NetStatsWidget::~NetStatsWidget() {
    m_netMeasure->postUpdate.detach(m_postUpdateHandle);
}

void NetStatsWidget::draw() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLines(RG_FONT_STANDARD, m_statsStrings, 0, 0, m_viewport.width, m_viewport.height,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 10);
}

PostUpdateEvent::Handle NetStatsWidget::RegisterPostUpdateCallback() {
    return m_netMeasure->postUpdate.attach([this]() {
        m_statsStrings.back() = std::string{ "Connection Status: " } + (m_netMeasure->isConnected() ? "Up" : "Down");
    });
}

} // namespace rg
