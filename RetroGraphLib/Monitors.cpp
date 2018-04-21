#include "Monitors.h"

#include "utils.h"

namespace rg {

Monitors::Monitors() {
    fillMonitorData();
}

void Monitors::fillMonitorData() {
    if (!EnumDisplayMonitors(nullptr, nullptr, MonitorCallback2, (LPARAM)this)) {
        fatalMessageBox("Failed to enumerate monitors");
    }
}

BOOL CALLBACK Monitors::MonitorCallback2(HMONITOR hMonitor, HDC, LPRECT, 
                                         LPARAM dwData) {
    const auto This{ (Monitors*)dwData };
    static int32_t monitorCount{ 0 };

    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &mi);

    This->m_monitors.emplace_back(monitorCount, hMonitor,
            mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top,
            mi.rcWork.left, mi.rcWork.top);

    ++monitorCount;
    return TRUE;
}


}
