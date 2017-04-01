#pragma once

#include <vector>
#include <stdint.h>
#include <Windows.h>

namespace rg {

struct MonitorData {
    MonitorData(int32_t _index, HMONITOR _handle, 
                int32_t _width, int32_t _height, int32_t _x, int32_t _y) : 
        index{ _index }, handle{ _handle },
        width{ _width }, height{ _height }, x{ _x }, y{ _y } { /* Empty */ }

    ~MonitorData() noexcept = default;

    int32_t index;
    int32_t width;
    int32_t height;
    int32_t x;
    int32_t y;
    HMONITOR handle;
};

class Monitors {
public:
    Monitors();
    ~Monitors() noexcept = default;
    Monitors(const Monitors&) = delete;
    Monitors& operator=(const Monitors&) = delete;

    const std::vector<MonitorData>& getMonitorData() const { return m_monitors; }
    size_t getNumMonitors() const { return m_monitors.size(); }

private:
    void fillMonitorData();

    static BOOL CALLBACK MonitorCallback2(HMONITOR hMonitor, 
            HDC, LPRECT, LPARAM dwData);


    std::vector<MonitorData> m_monitors{ };
};

}
