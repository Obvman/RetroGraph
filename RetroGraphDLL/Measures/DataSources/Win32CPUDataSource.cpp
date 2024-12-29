module RG.Measures.DataSources:Win32CPUDataSource;

import :NtDefs;

import std.compat;

import RG.Core;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

#pragma comment(lib, "PowrProf.lib")

namespace rg {

static uint64_t fileTimeToInt(const FILETIME& ft) {
    return ((static_cast<unsigned long long>(ft.dwHighDateTime)) << 32) |
           (static_cast<unsigned long long>(ft.dwLowDateTime));
}

Win32CPUDataSource::Win32CPUDataSource()
    : m_cpuName{ determineCPUName() }
    , m_numCores{ determineNumCores() }
    , m_cpuClockSpeed{ determineClockSpeed() }
    , m_cpuUsage{ getCPUUsage() } {}

void Win32CPUDataSource::update() {
    m_cpuClockSpeed = determineClockSpeed();
    m_cpuUsage = calculateCPUUsage();
}

std::string Win32CPUDataSource::determineCPUName() const {
    // Credit to bsruth -
    // http://stackoverflow.com/questions/850774/how-to-determine-the-hardware-cpu-and-ram-on-a-machine
    int cpuInfo[4] = { -1 };
    // Get the information associated with each extended ID.
    __cpuid(cpuInfo, 0x80000000);
    const auto nExIds{ cpuInfo[0] };
    char cpuBrandString[0x40];
    for (int i = 0x80000000; i <= nExIds; ++i) {
        __cpuid(cpuInfo, i);
        // Interpret CPU brand string
        if (i == 0x80000002)
            memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000003)
            memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000004)
            memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
    }

    return trim(std::string{ cpuBrandString });
}

int Win32CPUDataSource::determineNumCores() const {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    return systemInfo.dwNumberOfProcessors;
}

float Win32CPUDataSource::determineClockSpeed() const {
    std::vector<PROCESSOR_POWER_INFORMATION> processorInformation(m_numCores);
    RGVERIFY(NT_SUCCESS(CallNtPowerInformation(ProcessorInformation, nullptr, 0, processorInformation.data(),
                                               m_numCores * sizeof(PROCESSOR_POWER_INFORMATION))),
             "Failed to get processor information");
    return static_cast<float>(processorInformation[0].CurrentMhz);
}

float Win32CPUDataSource::calculateCPUUsage() const {
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }

    const uint64_t idleTicks{ fileTimeToInt(idleTime) };
    const uint64_t totalTicks{ fileTimeToInt(kernelTime) + fileTimeToInt(userTime) };
    static uint64_t prevTotalTicks{ 0U };
    static uint64_t prevIdleTicks{ 0U };

    const uint64_t totalTicksSinceLastTime{ totalTicks - prevTotalTicks };
    const uint64_t idleTicksSinceLastTime{ idleTicks - prevIdleTicks };

    const float cpuLoad{ 1.0f - ((totalTicksSinceLastTime > 0)
                                     ? (static_cast<float>(idleTicksSinceLastTime)) / totalTicksSinceLastTime
                                     : 0) };

    prevTotalTicks = totalTicks;
    prevIdleTicks = idleTicks;

    return cpuLoad;
}

} // namespace rg
