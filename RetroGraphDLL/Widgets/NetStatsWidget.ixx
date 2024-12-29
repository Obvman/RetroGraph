export module RG.Widgets:NetStatsWidget;

import :Widget;

import RG.Measures;
import RG.Rendering;

import std;

namespace rg {

export class NetStatsWidget : public Widget {
public:
    NetStatsWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure);
    ~NetStatsWidget() noexcept;

    void draw() const override;

private:
    ConnectionStatusChangedEvent::Handle RegisterNetConnectionStatusChangedCallback();
    BestAdapterChangedEvent::Handle RegisterBestAdapterChangedCallback();

    std::shared_ptr<const NetMeasure> m_netMeasure;
    std::vector<std::string> m_statsStrings;
    ConnectionStatusChangedEvent::Handle m_netConnectionStatusChangedHandle;
    BestAdapterChangedEvent::Handle m_bestAdapterChangedHandle;
};

} // namespace rg
