export module Measures.RAMMeasure;

import Units;

import Measures.Measure;

import std.core;

import "EventppHeaderUnit.h";
import "WindowsHeaderUnit.h";

namespace rg {

export using RAMUsageCallbackList = eventpp::CallbackList<void (float)>;
export using RAMUsageCallbackHandle = RAMUsageCallbackList::Handle;

/* Stores capacity totals and availability for system RAM */
export class RAMMeasure : public Measure {
public:
    RAMMeasure();
    ~RAMMeasure() noexcept = default;

    /* Updates the system memory status values */
    void update() override;

    std::chrono::microseconds updateInterval() const override { return std::chrono::seconds{ 1 }; }

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

    mutable RAMUsageCallbackList onRAMUsage;

private:
    /* Returns more accurate load percentage as a float from 0.0 - 1.0 */
    float getLoadPercentagef() const;

    MEMORYSTATUSEX m_memStatus{ };
};

} // namespace rg
