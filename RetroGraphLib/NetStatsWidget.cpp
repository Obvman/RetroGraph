#include "stdafx.h"
#include "NetStatsWidget.h"

#include "drawUtils.h"
#include "colors.h"
#include "FontManager.h"
#include "NetMeasure.h"

namespace rg {


NetStatsWidget::NetStatsWidget(const FontManager* fontManager, const RetroGraph& rg, bool visible)
    : Widget{ fontManager, visible }
    , m_netMeasure{ &rg.getNetMeasure() } {

    m_statsStrings.emplace_back("Hostname: " + m_netMeasure->getHostname());
    m_statsStrings.emplace_back("LAN IP: " + m_netMeasure->getAdapterIP());
    m_statsStrings.emplace_back("DNS: " + m_netMeasure->getDNS());
    m_statsStrings.emplace_back("MAC: " + m_netMeasure->getAdapterMAC());
}

void NetStatsWidget::draw() const {
    if (!isVisible()) return;

    clear();
    drawWidgetBackground();

    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    glLineWidth(0.5f);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    auto statsStrings{ m_statsStrings };
    std::string state{ (m_netMeasure->isConnected() ? "Up" : "Down") };
    statsStrings.emplace_back("Connection Status: " + state);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLines(RG_FONT_STANDARD, statsStrings, 0, 0,
                               m_viewport.width, m_viewport.height,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 10);
}

}
