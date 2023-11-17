export module Measures.RAMMeasure;

import Colors;
import Units;
import UserSettings;
import Utils;

import Measures.Measure;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

/* Stores capacity totals and availability for system RAM */
export class RAMMeasure : public Measure {
public:
    RAMMeasure();
    ~RAMMeasure() noexcept = default;

    /* Updates the system memory status values */
    void update(int ticks) override;
    void refreshSettings() override;

    /* Gets the total size of the system's physical memory in different
       byte units */
    DWORDLONG getTotalPhysicalB() const { return m_memStatus.ullTotalPhys; }
    DWORDLONG getTotalPhysicalKB() const { return bToKB(m_memStatus.ullTotalPhys); }
    DWORDLONG getTotalPhysicalMB() const { return bToMB(m_memStatus.ullTotalPhys); }
    float getTotalPhysicalGB() const { return m_memStatus.ullTotalPhys / static_cast<float>(GB); }

    /* Gets the remaining available amount of the system's physical memory in
       different byte units */
    DWORDLONG getAvailablePhysicalB() const { return m_memStatus.ullAvailPhys; }
    DWORDLONG getAvailablePhysicalKB() const { return m_memStatus.ullAvailPhys / KB; }
    DWORDLONG getAvailablePhysicalMB() const { return m_memStatus.ullAvailPhys / MB; }
    float getAvailablePhysicalGB() const { return m_memStatus.ullAvailPhys / static_cast<float>(GB); }

    /* Gets the currently used amount of the system's physical memory in
       different byte units */
    DWORDLONG getUsedPhysicalB() const { return getTotalPhysicalB() - getAvailablePhysicalB(); }
    DWORDLONG getUsedPhysicalKB() const { return getTotalPhysicalKB() - getAvailablePhysicalKB(); }
    DWORDLONG getUsedPhysicalMB() const { return getTotalPhysicalMB() - getAvailablePhysicalMB(); }
    float getUsedPhysicalGB() const { return getTotalPhysicalGB() - getAvailablePhysicalGB(); }

    /* Returns memory load as integer from 0 - 100 */
    int getLoadPercentage() const { return m_memStatus.dwMemoryLoad; }

    const std::vector<float> getUsageData() const { return m_usageData; }

private:
    /* Returns more accurate load percentage as a float from 0.0 - 1.0 */
    float getLoadPercentagef() const;

    MEMORYSTATUSEX m_memStatus{ };
    int dataSize{ 40 }; // max number of usage percentages to store
    std::vector<float> m_usageData{ };
};

RAMMeasure::RAMMeasure()
    : dataSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") } {

    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);

    m_usageData.assign(dataSize, 0.0f);
}

void RAMMeasure::update(int) {
    GlobalMemoryStatusEx(&m_memStatus);

    // Add value to the list of load values and shift the list left
    m_usageData.front() = getLoadPercentagef();
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());
}

void RAMMeasure::refreshSettings() {
    const int newDataSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") };
    if (dataSize == newDataSize)
        return;

    m_usageData.assign(newDataSize, 0.0f);
    dataSize = newDataSize;
}

float RAMMeasure::getLoadPercentagef() const {
    return (static_cast<float>(getUsedPhysicalB()) /
            static_cast<float>(getTotalPhysicalB()));
}

}
