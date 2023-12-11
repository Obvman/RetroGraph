module Widgets.NetGraphWidget;

import Colors;
import Units;

namespace rg {

NetGraphWidget::NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure)
    : Widget{ fontManager }
    , m_netMeasure{ netMeasure }
    , m_onDownBytesHandle{ RegisterNetDownBytesCallback() }
    , m_onUpBytesHandle{ RegisterNetUpBytesCallback() }
    , m_configRefreshedHandle{ RegisterConfigRefreshedCallback() }
    , m_graphSampleSize{ UserSettings::inst().getVal<int>("Widgets-NetGraph.NumUsageSamples") }
    , m_netGraph{ static_cast<size_t> (m_graphSampleSize) }
    , m_downLowerBound{ KB * UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.DownloadDataScaleLowerBoundKB") }
    , m_upLowerBound{ KB * UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.UploadDataScaleLowerBoundKB") }
    , m_maxDownValue{ m_downLowerBound }
    , m_maxUpValue{ m_upLowerBound }
    , m_downBytes( static_cast<size_t> (m_graphSampleSize), 0 )
    , m_upBytes( static_cast<size_t> (m_graphSampleSize), 0 ) {
}

NetGraphWidget::~NetGraphWidget() {
    UserSettings::inst().configRefreshed.remove(m_configRefreshedHandle);
    m_netMeasure->onUpBytes.remove(m_onUpBytesHandle);
    m_netMeasure->onDownBytes.remove(m_onDownBytesHandle);
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

NetUsageCallbackHandle NetGraphWidget::RegisterNetDownBytesCallback() {
    return m_netMeasure->onDownBytes.append(
        [this](int64_t downBytes) {

            // Remove old data and add the new to ensure queue maintains fixed size
            m_downBytes.pop_front();
            m_downBytes.push_back(downBytes);

            // Calculate the maximum value in the dataset to determine the scale to use.
            int64_t downMaxVal{ *std::max_element(m_downBytes.cbegin(), m_downBytes.cend()) };

            // Make sure the scale never goes lower than the lower bound set by the user.
            downMaxVal = std::max(downMaxVal, m_downLowerBound);

            bool scaleChanged{ false };
            if (m_maxDownValue != downMaxVal) {
                m_maxDownValue = downMaxVal;
                scaleChanged = true;
            }

            const auto maxDownValMB{ m_maxDownValue / static_cast<float>(MB) };
            if (scaleChanged) {
                std::vector<float> normalizedDownData;
                normalizedDownData.reserve(m_downBytes.size());
                for (const auto down : m_downBytes) {
                    normalizedDownData.push_back((down / static_cast<float>(MB)) / maxDownValMB);
                }
                // If the scale of our data changed then we need to set all of the points again in the graph
                // so can recalculate the whole curve
                m_netGraph.setTopPoints(normalizedDownData);
            } else {
                float normalizedDownMB{ (m_downBytes.back() / static_cast<float>(MB)) / maxDownValMB };
                m_netGraph.addTopPoint(normalizedDownMB);
            }
        });
}

NetUsageCallbackHandle NetGraphWidget::RegisterNetUpBytesCallback() {
    return m_netMeasure->onUpBytes.append(
        [this](int64_t upBytes) {
            // #TODO duplication with downbyte handling above.

            // Remove old data and add the new to ensure queue maintains fixed size
            m_upBytes.pop_front();
            m_upBytes.push_back(upBytes);

            // Calculate the maximum value in the dataset to determine the scale to use.
            int64_t upMaxVal{ *std::max_element(m_upBytes.cbegin(), m_upBytes.cend()) };

            // Make sure the scale never goes lower than the lower bound set by the user.
            upMaxVal = std::max(upMaxVal, m_upLowerBound);

            bool scaleChanged{ false };
            if (m_maxUpValue != upMaxVal) {
                m_maxUpValue = upMaxVal;
                scaleChanged = true;
            }

            const auto maxUpValMB{ m_maxUpValue / static_cast<float>(MB) };
            if (scaleChanged) {
                std::vector<float> normalizedUpData;
                normalizedUpData.reserve(m_upBytes.size());
                for (const auto up : m_upBytes) {
                    normalizedUpData.push_back((up / static_cast<float>(MB)) / maxUpValMB);
                }
                // If the scale of our data changed then we need to set all of the points again in the graph
                // so can recalculate the whole curve
                m_netGraph.setBottomPoints(normalizedUpData);
            } else {
                float normalizedUpMB{ (m_upBytes.back() / static_cast<float>(MB)) / maxUpValMB };
                m_netGraph.addBottomPoint(normalizedUpMB);
            }
        });
}

ConfigRefreshedCallbackHandle NetGraphWidget::RegisterConfigRefreshedCallback() {
    return UserSettings::inst().configRefreshed.append(
        [this]() {
            m_downLowerBound = KB * UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.DownloadDataScaleLowerBoundKB");
            m_upLowerBound = KB * UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.UploadDataScaleLowerBoundKB");

            const int newGraphSampleSize{ UserSettings::inst().getVal<int>("Widgets-NetGraph.NumUsageSamples") };
            if (m_graphSampleSize != newGraphSampleSize) {
                m_graphSampleSize = newGraphSampleSize;
                m_netGraph.resetPoints(m_graphSampleSize);
                m_maxDownValue = m_downLowerBound;
                m_maxUpValue = m_upLowerBound;
                m_downBytes = NetBytesQueue(static_cast<size_t>(m_graphSampleSize), 0);
                m_upBytes = NetBytesQueue(static_cast<size_t>(m_graphSampleSize), 0);
            }
        }
    );
}

} // namespace rg
