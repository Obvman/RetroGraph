#pragma once
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "utils.h"

namespace rg {

class RAMMeasure {
public:
    RAMMeasure(GLint startX, GLint startY, GLint ramWidth, GLint ramHeight);
    ~RAMMeasure();

    /* Updates the system memory status values */
    void update();

    /* Draws the components of this object */
    void draw() const;

    /* Draws the RAM usage bar */
    void drawBar() const;

    /* Draws the RAM usage text */
    void drawText() const;

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
    DWORD getLoadPercentage() const { return m_memStatus.dwMemoryLoad; }
private:
    MEMORYSTATUSEX m_memStatus;

    // Rendering members
    GLint m_viewportStartX;
    GLint m_viewportStartY;
    GLint m_viewportWidth;
    GLint m_viewportHeight;
};

}