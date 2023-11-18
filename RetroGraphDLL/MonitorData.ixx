export module MonitorData;

import "WindowsHeaderUnit.h";

namespace rg {

export struct MonitorData {
    MonitorData(int _index, HMONITOR _handle, int _realWidth, int _realHeight,
                int _width, int _height, int _x, int _y, int _refreshRate)
        : index{ _index }
        , realWidth{ _realWidth }
        , realHeight{ _realHeight }
        , width{ _width }
        , height{ _height }
        , x{ _x }
        , y{ _y }
        , refreshRate{ _refreshRate }
        , handle{ _handle } { /* Empty */ }

    ~MonitorData() noexcept = default;
    MonitorData(const MonitorData&) = default;
    MonitorData& operator=(const MonitorData&) = default;
    MonitorData(MonitorData&&) = default;
    MonitorData& operator=(MonitorData&&) = default;

    int index;

    // Total pixels
    int realWidth;
    int realHeight;

    // Work area (not including taskbar)
    int width;
    int height;

    int x;
    int y;

    int refreshRate;

    HMONITOR handle;
};

} // namespace rg
