export module RG.Widgets:NetGraphWidget;

import :Widget;

import ConfigRefreshedEvent;

import RG.Measures;
import RG.Rendering;
import RG.Widgets.Graph;

import std.core;

namespace rg {

using NetBytesQueue = std::deque<int64_t>;

export class NetGraphWidget : public Widget {
public:
    NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure);
    ~NetGraphWidget();

    void draw() const override;

private:
    std::string getScaleLabel(int64_t bytesTransferred) const;
    void addUsageValue(NetBytesQueue& usageQueue, LineGraph& graph,
                       int64_t& currentMaxValue, int64_t lowerBound, int64_t usageValue);

    NetUsageEvent::Handle RegisterNetDownBytesCallback();
    NetUsageEvent::Handle RegisterNetUpBytesCallback();
    ConfigRefreshedEvent::Handle RegisterConfigRefreshedCallback();

    std::shared_ptr<const NetMeasure> m_netMeasure;
    NetUsageEvent::Handle m_onDownBytesHandle;
    NetUsageEvent::Handle m_onUpBytesHandle;
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;

    int m_graphSampleSize;
    SmoothMirrorLineGraph m_netGraph;

    int64_t m_downLowerBound;
    int64_t m_upLowerBound;
    int64_t m_maxDownValue;
    int64_t m_maxUpValue;
    NetBytesQueue m_downBytes;
    NetBytesQueue m_upBytes;
};

}
