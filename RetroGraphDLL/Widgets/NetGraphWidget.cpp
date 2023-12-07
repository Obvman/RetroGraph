module Widgets.NetGraphWidget;

import Colors;
import Units;

namespace rg {

NetGraphWidget::NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure)
    : Widget{ fontManager }
    , m_netMeasure{ netMeasure }
    , m_netGraph{ netMeasure->getUpData().size(), netMeasure->getDownData().size() }
    , m_maxDownValue{ netMeasure->getMaxDownValue() }
    , m_maxUpValue{ netMeasure->getMaxUpValue() }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() }
    , m_downLowerBound{ UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.DownloadDataScaleLowerBoundKB")}
    , m_upLowerBound{ UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.UploadDataScaleLowerBoundKB")}
    , m_configChangedHandle{
        UserSettings::inst().configChanged.append(
            [&]() {
                m_downLowerBound = UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.DownloadDataScaleLowerBoundKB");
                m_upLowerBound = UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.UploadDataScaleLowerBoundKB");
            })
    } {
}

NetGraphWidget::~NetGraphWidget() {
    UserSettings::inst().configChanged.remove(m_configChangedHandle);
    m_netMeasure->postUpdate.remove(m_postUpdateHandle);
}

void NetGraphWidget::draw() const {

    {// Draw the line graphs
        glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4) / 5, m_viewport.height);
        m_netGraph.draw();
    }

    {// Text
        glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y, m_viewport.width / 5, m_viewport.height);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        m_fontManager->renderLine(RG_FONT_SMALL, getScaleLabel(m_maxDownValue), 0, 0, m_viewport.width / 5, m_viewport.height,
                                  RG_ALIGN_TOP | RG_ALIGN_LEFT);

        m_fontManager->renderLine(RG_FONT_SMALL, "Down / Up", 0, 0, m_viewport.width / 5, m_viewport.height,
                                  RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT);
        m_fontManager->renderLine(RG_FONT_SMALL, getScaleLabel(m_maxUpValue), 0, 0, m_viewport.width / 5, m_viewport.height,
                                  RG_ALIGN_BOTTOM | RG_ALIGN_LEFT);
    }
}

std::string NetGraphWidget::getScaleLabel(int64_t bytesTransferred) const {
    const std::string_view suffix = [bytesTransferred]() {
        if (bytesTransferred > 1000 * 1000)
            return "MB";
        else if (bytesTransferred > 1000)
            return "KB";
        else
            return "B";
    }(); // Call immediately

    if (suffix == "MB") {
        char buff[8];
        snprintf(buff, sizeof(buff), "%5.1fMB", bytesTransferred / static_cast<float>(MB));
        return buff;
    } else if (suffix == "KB") {
        char buff[8];
        snprintf(buff, sizeof(buff), "%5.1fKB", bytesTransferred / static_cast<float>(KB));
        return buff;
    } else {
        char buff[5];
        snprintf(buff, sizeof(buff), "%3lluB", bytesTransferred);
        return buff;
    }
}

PostUpdateCallbackHandle NetGraphWidget::RegisterPostUpdateCallback() {
    return m_netMeasure->postUpdate.append(
        [this]() {
            const auto& downData{ m_netMeasure->getDownData() };
            const auto& upData{ m_netMeasure->getUpData() };

            bool scaleChanged{ false };
            if (m_maxDownValue != m_netMeasure->getMaxDownValue()) {
                m_maxDownValue = std::max(m_netMeasure->getMaxDownValue(), m_downLowerBound * KB);
                scaleChanged = true;
            }
            if (m_maxUpValue != m_netMeasure->getMaxUpValue()) {
                m_maxUpValue = std::max(m_netMeasure->getMaxUpValue(), m_upLowerBound * KB);
                scaleChanged = true;
            }

            const auto maxDownValMB{ m_maxDownValue / static_cast<float>(MB) };
            const auto maxUpValMB{ m_maxUpValue / static_cast<float>(MB) };

            std::vector<float> normalizedDownData(downData.size());
            for (auto i = int{ 0 }; i < downData.size(); ++i) {
                normalizedDownData[i] = (downData[i] / static_cast<float>(MB)) / maxDownValMB;
            }
            std::vector<float> normalizedUpData(upData.size());
            for (auto i = int{ 0 }; i < upData.size(); ++i) {
                normalizedUpData[i] = (upData[i] / static_cast<float>(MB)) / maxUpValMB;
            }

            if (scaleChanged)
                m_netGraph.resetPoints(normalizedDownData, normalizedUpData);
            else
                m_netGraph.updatePoints(normalizedDownData, normalizedUpData);
        });
}

} // namespace rg
