#pragma once

#include <stdint.h>
#include <vector>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "Measure.h"
#include "utils.h"

namespace rg {

/* Stores capacity totals and availability for system RAM */
class RAMMeasure : public Measure {
public:
    RAMMeasure();
    ~RAMMeasure();
    RAMMeasure(const RAMMeasure&) = delete;
    RAMMeasure& operator=(const RAMMeasure&) = delete;

    virtual void init();

    /* Updates the system memory status values */
    virtual void update(uint32_t ticks);

    /* Draws the components of this object */
    void draw() const;

    /* Gets the total size of the system's physical memory in different
       byte units */
    DWORDLONG getTotalPhysicalB() const { return m_memStatus.ullTotalPhys; }
    DWORDLONG getTotalPhysicalKB() const { return m_memStatus.ullTotalPhys / KB; }
    DWORDLONG getTotalPhysicalMB() const { return m_memStatus.ullTotalPhys / MB; }
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
    uint32_t getLoadPercentage() const { return m_memStatus.dwMemoryLoad; }

    const std::vector<float> getUsageData() const { return m_usageData; }

private:
    /* Draws the RAM usage bar */
    void drawBar() const;

    /* Draws the RAM usage text */
    void drawText() const;

    /* Draws the RAM usage line graph */
    void drawGraph() const;

    /* Returns more accurate load percentage as a float from 0.0 - 1.0 */
    float getLoadPercentagef() const;

    MEMORYSTATUSEX m_memStatus;
    size_t dataSize; // max number of usage percentages to store
    std::vector<float> m_usageData;

};

}