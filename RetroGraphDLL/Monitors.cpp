module Monitors;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

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

    This->m_monitors.emplace_back(monitorCount, hMonitor,
            mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top,
            mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top,
            mi.rcWork.left, mi.rcWork.top, refresh);

    ++monitorCount;
    return TRUE;
}

} // namespace rg
