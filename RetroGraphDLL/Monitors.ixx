export module Monitors;

import MonitorData;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

export class Monitors {
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
