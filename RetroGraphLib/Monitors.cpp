#include "stdafx.h"

#include "Monitors.h"

#include "utils.h"

namespace rg {

Monitors::Monitors() {
    fillMonitorData();
}

void Monitors::fillMonitorData() {
    if (!EnumDisplayMonitors(nullptr, nullptr, MonitorCallback2, reinterpret_cast<LPARAM>(this))) {
        fatalMessageBox("Failed to enumerate monitors");
    }
}

BOOL CALLBACK Monitors::MonitorCallback2(HMONITOR hMonitor, HDC, LPRECT,
                                         LPARAM dwData) {
    static int32_t monitorCount{ 0 };

    auto* This{ reinterpret_cast<Monitors*>(dwData) };

    MONITORINFOEX mi;
    mi.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, &mi);

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    int32_t refresh{ 0 };
    if (EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm)) {
        refresh = dm.dmDisplayFrequency;
    }

    This->m_monitors.emplace_back(monitorCount, hMonitor,
            mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
            mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top,
            mi.rcWork.left, mi.rcWork.top, refresh);

    ++monitorCount;
    return TRUE;
}


}
