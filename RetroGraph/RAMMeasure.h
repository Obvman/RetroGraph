#pragma once
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace rg {

class RAMMeasure {
public:
    RAMMeasure(GLint ramWidth, GLint ramHeight, GLint startX, GLint startY);
    ~RAMMeasure();

    void update();
    void draw() const;
    void drawBar() const;
    void drawText() const;

    DWORDLONG getTotalPhysicalB() const { return m_memStatus.ullTotalPhys; }
    DWORDLONG getTotalPhysicalKB() const { return m_memStatus.ullTotalPhys / 1024; }
    DWORDLONG getTotalPhysicalMB() const { return m_memStatus.ullTotalPhys / (1024 * 1024); }
    float getTotalPhysicalGB() const { return m_memStatus.ullTotalPhys / (1024.0f * 1024.0f * 1024.0f); }

    DWORDLONG getAvailablePhysicalB() const { return m_memStatus.ullAvailPhys; }
    DWORDLONG getAvailablePhysicalKB() const { return m_memStatus.ullAvailPhys / 1024; }
    DWORDLONG getAvailablePhysicalMB() const { return m_memStatus.ullAvailPhys / (1024 * 1024); }
    float getAvailablePhysicalGB() const { return m_memStatus.ullAvailPhys / (1024.0f * 1024.0f * 1024.0f); }

    DWORDLONG getUsedPhysicalB() const { return getTotalPhysicalB() - getAvailablePhysicalB(); }
    DWORDLONG getUsedPhysicalKB() const { return getTotalPhysicalKB() - getAvailablePhysicalKB(); }
    DWORDLONG getUsedPhysicalMB() const { return getTotalPhysicalMB() - getAvailablePhysicalMB(); }
    float getUsedPhysicalGB() const { return getTotalPhysicalGB() - getAvailablePhysicalGB(); }

    // Returns memory load as integer from 0 - 100
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