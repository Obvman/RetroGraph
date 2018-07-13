#pragma once

#include "stdafx.h"

#include <Windows.h>

#include <vector>

#include <cstdint>

namespace rg {

struct MonitorData {
    MonitorData(int32_t _index, HMONITOR _handle, int32_t _realWidth, int32_t _realHeight,
                int32_t _width, int32_t _height, int32_t _x, int32_t _y, int32_t _refreshRate)
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

    int32_t index;

    // Total pixels
    int32_t realWidth;
    int32_t realHeight;

    // Work area (not including taskbar)
    int32_t width;
    int32_t height;

    int32_t x;
    int32_t y;

    int32_t refreshRate;

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

    int32_t getWidth(size_t i) const { return m_monitors[i].width; }
    int32_t getHeight(size_t i) const { return m_monitors[i].height; }
    int32_t getX(size_t i) const { return m_monitors[i].x; }
    int32_t getY(size_t i) const { return m_monitors[i].y; }
private:
    void fillMonitorData();

    static BOOL CALLBACK MonitorCallback2(HMONITOR hMonitor,
            HDC, LPRECT, LPARAM dwData);

    std::vector<MonitorData> m_monitors{ };
};

} // namespace rg
