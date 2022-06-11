module;

#include "RGAssert.h"

export module Monitors;

import Utils;

import <vector>;

import <WindowsHeaders.h>;

namespace rg {

export class Monitors;

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

Monitors::Monitors() {
    fillMonitorData();
}

void Monitors::fillMonitorData() {
    RGVERIFY(EnumDisplayMonitors(nullptr, nullptr, MonitorCallback2, reinterpret_cast<LPARAM>(this)),
             "Failed to enumerate monitors");
}

BOOL CALLBACK Monitors::MonitorCallback2(HMONITOR hMonitor, HDC, LPRECT,
                                         LPARAM dwData) {
    static auto monitorCount = int{ 0 };

    auto* This{ reinterpret_cast<Monitors*>(dwData) };

    MONITORINFOEX mi;
    mi.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &mi);

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    auto refresh = int{ 0 };
    if (EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm)) {
        refresh = dm.dmDisplayFrequency;
    }

    // #TODO this gives incorrect results for 4K monitor on the left of the main monitor.
    This->m_monitors.emplace_back(monitorCount, hMonitor,
            mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
            mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top,
            mi.rcWork.left, mi.rcWork.top, refresh);

    ++monitorCount;
    return TRUE;
}

}
