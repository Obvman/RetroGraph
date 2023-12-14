export module Widgets.TimeWidget;

import Measures.NetMeasure;
import Measures.TimeMeasure;

import Rendering.FontManager;

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

    PostUpdateCallbackHandle RegisterTimePostUpdateCallback();
    NetConnectionStatusChangedCallbackHandle RegisterNetConnectionStatusChangedCallback();

    std::shared_ptr<const TimeMeasure> m_timeMeasure;
    std::shared_ptr<const NetMeasure> m_netMeasure;

    PostUpdateCallbackHandle m_timePostUpdateHandle;
    NetConnectionStatusChangedCallbackHandle m_netConnectionStatusChangedHandle;
};

} // namespace rg
