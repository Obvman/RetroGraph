module RG.Widgets:NetGraphWidget;

import Colors;

import RG.Core;

namespace rg {

NetGraphWidget::NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure)
    : Widget{ fontManager }
    , m_netMeasure{ netMeasure }
    , m_onDownBytesHandle{ RegisterNetDownBytesCallback() }
    , m_onUpBytesHandle{ RegisterNetUpBytesCallback() }
    , m_configRefreshedHandle{ RegisterConfigRefreshedCallback() }
    , m_graphSampleSize{ UserSettings::inst().getVal<int>("Widgets-NetGraph.NumUsageSamples") }
    , m_netGraph{ static_cast<size_t>(m_graphSampleSize) }
    , m_downLowerBound{ KB *
                        UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.DownloadDataScaleLowerBoundKB") }
    , m_upLowerBound{ KB * UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.UploadDataScaleLowerBoundKB") }
    , m_maxDownValue{ m_downLowerBound }
    , m_maxUpValue{ m_upLowerBound }
    , m_downBytes(static_cast<size_t>(m_graphSampleSize), 0)
    , m_upBytes(static_cast<size_t>(m_graphSampleSize), 0) {}

NetGraphWidget::~NetGraphWidget() {
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
    m_netMeasure->onUpBytes.detach(m_onUpBytesHandle);
    m_netMeasure->onDownBytes.detach(m_onDownBytesHandle);
}

void NetGraphWidget::draw() const {
    { // Draw the line graphs
        glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4) / 5, m_viewport.height);
        m_netGraph.draw();
    }

    { // Text
        glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y, m_viewport.width / 5, m_viewport.height);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        m_fontManager->renderLine(RG_FONT_SMALL, getScaleLabel(m_maxDownValue), 0, 0, m_viewport.width / 5,
                                  m_viewport.height, RG_ALIGN_TOP | RG_ALIGN_LEFT);

        m_fontManager->renderLine(RG_FONT_SMALL, "Down / Up", 0, 0, m_viewport.width / 5, m_viewport.height,
                                  RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT);
        m_fontManager->renderLine(RG_FONT_SMALL, getScaleLabel(m_maxUpValue), 0, 0, m_viewport.width / 5,
                                  m_viewport.height, RG_ALIGN_BOTTOM | RG_ALIGN_LEFT);
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

void NetGraphWidget::addUsageValue(NetBytesQueue& usageQueue, LineGraph& graph, int64_t& currentMaxValue,
                                   int64_t lowerBound, int64_t usageValue) {
    // Remove old data and add the new to ensure queue maintains fixed size
    usageQueue.pop_front();
    usageQueue.push_back(usageValue);

    // Calculate the maximum value in the dataset to determine the scale to use.
    int64_t upValue{ *std::max_element(usageQueue.cbegin(), usageQueue.cend()) };

    // Make sure the scale never goes lower than the lower bound set by the user.
    upValue = std::max(upValue, lowerBound);

    bool scaleChanged{ false };
    if (currentMaxValue != upValue) {
        currentMaxValue = upValue;
        scaleChanged = true;
    }

    const auto maxValueMB{ currentMaxValue / static_cast<float>(MB) };
    if (scaleChanged) {
        std::vector<float> normalizedData;
        normalizedData.reserve(usageQueue.size());
        for (const auto up : usageQueue) {
            normalizedData.push_back((up / static_cast<float>(MB)) / maxValueMB);
        }
        // If the scale of our data changed then we need to set all of the points again in the graph
        // so can recalculate the whole curve
        graph.setPoints(normalizedData);
    } else {
        float normalizedUpMB{ (usageQueue.back() / static_cast<float>(MB)) / maxValueMB };
        graph.addPoint(normalizedUpMB);
    }

    invalidate();
}

NetUsageEvent::Handle NetGraphWidget::RegisterNetDownBytesCallback() {
    return m_netMeasure->onDownBytes.attach([this](int64_t downBytes) {
        addUsageValue(m_downBytes, m_netGraph.topGraph(), m_maxDownValue, m_downLowerBound, downBytes);
    });
}

NetUsageEvent::Handle NetGraphWidget::RegisterNetUpBytesCallback() {
    return m_netMeasure->onUpBytes.attach([this](int64_t upBytes) {
        addUsageValue(m_upBytes, m_netGraph.bottomGraph(), m_maxUpValue, m_upLowerBound, upBytes);
    });
}

ConfigRefreshedEvent::Handle NetGraphWidget::RegisterConfigRefreshedCallback() {
    return UserSettings::inst().configRefreshed.attach([this]() {
        m_downLowerBound =
            KB * UserSettings::inst().getVal<int, int64_t>("Widgets-NetGraph.DownloadDataScaleLowerBoundKB");
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
        invalidate();
    });
}

} // namespace rg
