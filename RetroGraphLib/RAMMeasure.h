#pragma once

#include "stdafx.h"

#include <cstdint>
#include <vector>
#include <Windows.h>

#include "units.h"

namespace rg {

class UserSettings;

/* Stores capacity totals and availability for system RAM */
class RAMMeasure {
public:
    RAMMeasure();
    ~RAMMeasure() noexcept = default;
    RAMMeasure(const RAMMeasure&) = delete;
    RAMMeasure& operator=(const RAMMeasure&) = delete;
    RAMMeasure(RAMMeasure&&) = delete;
    RAMMeasure& operator=(RAMMeasure&&) = delete;

    /* Updates the system memory status values */
    void update(uint32_t ticks);

    /* Gets the total size of the system's physical memory in different
       byte units */
    DWORDLONG getTotalPhysicalB() const {
        return m_memStatus.ullTotalPhys;
    }
    DWORDLONG getTotalPhysicalKB() const {
        return bToKB(m_memStatus.ullTotalPhys);
    }
    DWORDLONG getTotalPhysicalMB() const {
        return bToMB(m_memStatus.ullTotalPhys);
    }
    float getTotalPhysicalGB() const {
        return m_memStatus.ullTotalPhys / static_cast<float>(GB);
    }

    /* Gets the remaining available amount of the system's physical memory in
       different byte units */
    DWORDLONG getAvailablePhysicalB() const {
        return m_memStatus.ullAvailPhys;
    }
    DWORDLONG getAvailablePhysicalKB() const {
        return m_memStatus.ullAvailPhys / KB;
    }
    DWORDLONG getAvailablePhysicalMB() const {
        return m_memStatus.ullAvailPhys / MB;
    }
    float getAvailablePhysicalGB() const {
        return m_memStatus.ullAvailPhys / static_cast<float>(GB);
    }

    /* Gets the currently used amount of the system's physical memory in
       different byte units */
    DWORDLONG getUsedPhysicalB() const {
        return getTotalPhysicalB() - getAvailablePhysicalB();
    }
    DWORDLONG getUsedPhysicalKB() const {
        return getTotalPhysicalKB() - getAvailablePhysicalKB();
    }
    DWORDLONG getUsedPhysicalMB() const {
        return getTotalPhysicalMB() - getAvailablePhysicalMB();
    }
    float getUsedPhysicalGB() const {
        return getTotalPhysicalGB() - getAvailablePhysicalGB();
    }

    /* Returns memory load as integer from 0 - 100 */
    uint32_t getLoadPercentage() const { return m_memStatus.dwMemoryLoad; }

    const std::vector<float> getUsageData() const { return m_usageData; }

private:
    /* Returns more accurate load percentage as a float from 0.0 - 1.0 */
    float getLoadPercentagef() const;

    MEMORYSTATUSEX m_memStatus{ };
    size_t dataSize{ 40U }; // max number of usage percentages to store
    std::vector<float> m_usageData{ };
};

} // namespace rg
