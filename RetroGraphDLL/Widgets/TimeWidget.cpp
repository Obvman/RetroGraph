module Widgets.TimeWidget;

import Colors;

import Rendering.DrawUtils;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

TimeWidget::TimeWidget(const FontManager* fontManager,
                       std::shared_ptr<const TimeMeasure> timeMeasure,
                       std::shared_ptr<const NetMeasure> netMeasure)
    : Widget{ fontManager }
    , m_timeMeasure{ timeMeasure }
    , m_netMeasure{ netMeasure }
    , m_timePostUpdateHandle{ RegisterTimePostUpdateCallback() }
    , m_netConnectionStatusChangedHandle{ RegisterNetConnectionStatusChangedCallback() } {
}

TimeWidget::~TimeWidget() {
    m_netMeasure->onConnectionStatusChanged.remove(m_netConnectionStatusChangedHandle);
    m_timeMeasure->postUpdate.remove(m_timePostUpdateHandle);
}

void TimeWidget::draw() const {
    constexpr float leftDivX{ -0.33f };
    constexpr float rightDivX{ 0.33f };
    constexpr float midDivY{ -0.3f };
    const auto midDivYPx{ vpCoordsToPixels(midDivY, m_viewport.height) };

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);
    drawSerifLine(-0.9f, 0.9f, midDivY);

    glBegin(GL_LINES); {
        glVertex2f(leftDivX, viewportMin);
        glVertex2f(leftDivX, -0.3f); // Left vertical

        glVertex2f(rightDivX, viewportMin);
        glVertex2f(rightDivX, -0.3f); // Right vertical
    } glEnd();

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    const auto localTime{ m_timeMeasure->getLocalTime() };

    // Draw the big system time and the date below
    const std::string timeString{ std::format("{:%T}", localTime) };
    m_fontManager->renderLine(RG_FONT_TIME, timeString, 0, midDivYPx, m_viewport.width, m_viewport.height - midDivYPx,
                              RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_CENTERED_HORIZONTAL);

    // Draw the uptime in bottom-left
    m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, "Uptime", 0, 0,
                              m_viewport.width/3, midDivYPx, RG_ALIGN_RIGHT | RG_ALIGN_TOP, 10, 15);
    m_fontManager->renderLine(RG_FONT_STANDARD, getUptimeStr(),
                              0, 0, m_viewport.width/3, midDivYPx, RG_ALIGN_RIGHT | RG_ALIGN_BOTTOM, 10, 15);

    // Draw the year and month and day in bottom-middle
    const std::string dateString{ std::format("{:%d %B}", localTime) };
    const std::string dayString{ std::format("{:%A}", localTime) };
    m_fontManager->renderLine(RG_FONT_STANDARD, dateString,
                              vpCoordsToPixels(leftDivX, m_viewport.width), 0, m_viewport.width/3, midDivYPx,
                              RG_ALIGN_BOTTOM | RG_ALIGN_CENTERED_HORIZONTAL, 10, 15);
    m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, dayString,
                              vpCoordsToPixels(leftDivX, m_viewport.width), 0, m_viewport.width/3, midDivYPx,
                              RG_ALIGN_TOP | RG_ALIGN_CENTERED_HORIZONTAL, 10, 15);

    // Draw network connection status in bottom-right
    {
        // Get region to render text in
        const auto renderX{ vpCoordsToPixels(rightDivX, m_viewport.width) };
        const auto renderY{ 0 };
        const auto renderWidth{ m_viewport.width - renderX };
        const auto renderHeight{ vpCoordsToPixels(midDivY, m_viewport.height) };

        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, "Network", renderX, renderY, renderWidth, renderHeight,
                                  RG_ALIGN_LEFT | RG_ALIGN_TOP, 10, 15);

        const std::string netStatusString{ m_netMeasure->isConnected() ? "UP" : "DOWN" };
        m_fontManager->renderLine(RG_FONT_STANDARD, netStatusString, renderX, renderY, renderWidth, renderHeight,
                                  RG_ALIGN_LEFT | RG_ALIGN_BOTTOM, 10, 15);
    }
};

std::string TimeWidget::getUptimeStr() const {
    const std::chrono::seconds uptime{ m_timeMeasure->getUptime() };
    const auto uptimeS{ uptime % 60 };
    const auto uptimeM{ (uptime / 60) % 60 };
    const auto uptimeH{ (uptime / (60 * 60)) % 24 };
    const auto uptimeD{ (uptime / (60 * 60 * 24)) };

    return std::format("{:0>2}:{:0>2}:{:0>2}:{:0>2}",
                       uptimeD.count(), uptimeH.count(), uptimeM.count(), uptimeS.count());
}

PostUpdateCallbackHandle TimeWidget::RegisterTimePostUpdateCallback() {
    return m_timeMeasure->postUpdate.append(
        [this]() {
            invalidate();
        });
}

NetConnectionStatusChangedCallbackHandle TimeWidget::RegisterNetConnectionStatusChangedCallback() {
    return m_netMeasure->onConnectionStatusChanged.append(
        [this](bool /*connectionStatus*/) {
            invalidate();
        });
}

} // namespace rg
