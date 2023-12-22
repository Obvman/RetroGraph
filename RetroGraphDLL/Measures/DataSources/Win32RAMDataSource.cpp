module RG.Measures.DataSources:Win32RAMDataSource;

import "WindowsHeaderUnit.h";

namespace rg {

float Win32RAMDataSource::getRAMUsage() const {
    const auto memStatus{ getMemoryStatus() };
    const auto usedBytes{ memStatus.ullTotalPhys - memStatus.ullAvailPhys };
    return static_cast<float>(usedBytes) / memStatus.ullTotalPhys;
}

uint64_t Win32RAMDataSource::getRAMCapacity() const {
    return getMemoryStatus().ullTotalPhys;
}

MEMORYSTATUSEX Win32RAMDataSource::getMemoryStatus() const {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);
    return memStatus;
}

} // namespace rg
