module RG.Measures:SystemMeasure;

import "WindowsHeaderUnit.h";

namespace rg {

SystemMeasure::SystemMeasure(std::unique_ptr<const IOperatingSystemDataSource> operatingSystemDataSource)
    : Measure{ std::nullopt }
    , m_operatingSystemDataSource{ std::move(operatingSystemDataSource) } {

    const auto osData{ m_operatingSystemDataSource->getOperatingSystemData() };
    getCPUInfo();

    m_osName = osData.osName;
    m_osVersion = osData.osVersion;
    m_userName = osData.userName;
    m_computerName = osData.computerName;
}

void SystemMeasure::getCPUInfo() {

    // Credit to bsruth -
    // http://stackoverflow.com/questions/850774/how-to-determine-the-hardware-cpu-and-ram-on-a-machine
    int CPUInfo[4] = {-1};
    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    const auto nExIds{ CPUInfo[0] };
    char CPUBrandString[0x40];
    for (int i = 0x80000000; i <= nExIds; ++i) {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        if  (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }

    m_cpuDescription = "CPU: " + std::string{ CPUBrandString };
}

} // namespace rg
