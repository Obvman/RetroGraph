export module Widgets.TimeWidget;

import RG.Core;

import Measures.NetMeasure;
import Measures.TimeMeasure;

import RG.Rendering;

import Widgets.Widget;

import std.memory;

namespace rg {

export class TimeWidget : public Widget {
public:
    TimeWidget(const FontManager* fontManager,
               std::shared_ptr<const TimeMeasure> timeMeasure,
               std::shared_ptr<const NetMeasure> netMeasure);

    ~TimeWidget() noexcept;

    void draw() const override;

private:
    std::string getUptimeStr() const;

    PostUpdateEvent::Handle RegisterTimePostUpdateCallback();
    ConnectionStatusChangedEvent::Handle RegisterNetConnectionStatusChangedCallback();

    std::shared_ptr<const TimeMeasure> m_timeMeasure;
    std::shared_ptr<const NetMeasure> m_netMeasure;

    PostUpdateEvent::Handle m_timePostUpdateHandle;
    ConnectionStatusChangedEvent::Handle m_netConnectionStatusChangedHandle;
};

} // namespace rg
