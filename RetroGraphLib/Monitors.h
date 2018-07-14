#pragma once

#include "stdafx.h"

#include <Windows.h>

#include <vector>

#include <cstdint>

namespace rg {

struct MonitorData {
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

class Monitors {
public:
    Monitors();
    ~Monitors() noexcept = default;
    Monitors(const Monitors&) = delete;
    Monitors& operator=(const Monitors&) = delete;
    Monitors(Monitors&&) = delete;
    Monitors& operator=(Monitors&&) = delete;

    const std::vector<MonitorData>& getMonitorData() const { return m_monitors; }
    int getNumMonitors() const { return static_cast<int>(m_monitors.size()); }

    int getWidth(size_t i) const { return m_monitors[i].width; }
    int getHeight(size_t i) const { return m_monitors[i].height; }
    int getX(size_t i) const { return m_monitors[i].x; }
    int getY(size_t i) const { return m_monitors[i].y; }
private:
    void fillMonitorData();

    static BOOL CALLBACK MonitorCallback2(HMONITOR hMonitor,
            HDC, LPRECT, LPARAM dwData);

    std::vector<MonitorData> m_monitors{ };
};

} // namespace rg
