#include "CPUMeasure.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <time.h>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "Window.h"
#include "colors.h"
#include "utils.h"

static unsigned long long FileTimeToInt64(const FILETIME & ft) {return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);}

namespace rg {

CPUMeasure::CPUMeasure(Window* w, int32_t graphWidth, int32_t graphHeight) :
    m_parentWindow{ w },
    dataSize{ 80U },
    m_usageData{ },
    m_uptime{ std::chrono::milliseconds{GetTickCount64()} },
    m_viewportStartX{ 0 },
    m_viewportStartY{ 0 },
    m_viewportWidth{ graphWidth },
    m_viewportHeight{ graphHeight } {

    // fill vector with default values
    m_usageData.assign(dataSize, 0.5f);
}

CPUMeasure::~CPUMeasure() {
}

void CPUMeasure::update() {
    m_uptime = std::chrono::milliseconds(GetTickCount64());

    getCPULoad();
}

void CPUMeasure::draw(GLuint shader) const {
    glPushMatrix();

    // Preserve initial viewport settings
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    // Set up the view to a portion of the screen
    glViewport(m_viewportStartX, m_viewportStartY,
               m_viewportWidth, m_viewportHeight);

    glLineWidth(0.5f);

    drawGraph(shader);
    drawUptime();
    drawSystemTime();
    //drawGraphBox();
    drawViewportBorder();

    glViewport(vp[0], vp[1], vp[2], vp[3]);

    glPopMatrix();
}

void CPUMeasure::drawGraph(GLuint shader) const {
    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);
    glLineWidth(0.5f);

    //glUseProgram(shader);
    glBegin(GL_LINE_STRIP); {
        // Draw each node in the graph
        for (auto i{ 0U }; i < m_usageData.size(); ++i) {
            glColor4f(LINE_R, LINE_G, LINE_B, 1.0f);

            float x = (static_cast<float>(i) / (m_usageData.size() - 1)) * 2.0f - 1.0f;
            float y = m_usageData[i] * 2.0f - 1.0f;

            // If the vertex is at the border, add/subtract the border delta
            if (i == 0) {
                x += bDelta;
            }

            glVertex3f(x, y, 0.0f);
        }
    } glEnd();

    glUseProgram(0);

    glLineWidth(lineWidth);
}

void CPUMeasure::drawUptime() const {
    // Draw text
    const auto rasterX = float{ -0.95f };
    const auto rasterY = float{ 0.1f };
    auto uptime = "Uptime: " + getUptimeStr();

    glColor3f(TEXT_R, TEXT_G, TEXT_B);
    glRasterPos2f(rasterX, rasterY);
    glCallLists(uptime.size(), GL_UNSIGNED_BYTE, uptime.c_str());
}

void CPUMeasure::drawSystemTime() const {
    const auto rasterX = float{ -0.95f };
    const auto rasterY = float{ -0.1f };

    time_t now = time(0);
    tm t;
    char buf[9];
    localtime_s(&t, &now);
    strftime(buf, sizeof(buf), "%X", &t);

    std::stringstream ss;
    ss << "Time: " << buf;

    glColor3f(TEXT_R, TEXT_G, TEXT_B);
    glRasterPos2f(rasterX, rasterY);
    glCallLists(ss.str().length(), GL_UNSIGNED_BYTE, ss.str().c_str());
}

float CPUMeasure::getCPULoad() {
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }
    const auto load{ calculateCPULoad(FileTimeToInt64(idleTime),
                     FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime))
    };

    // TODO profile performance
    // option 1: shift elements left and add to the end afterwards
    // option 2: add then use std::rotate

    // Add to the usageData vector by overwriting the oldest value and
    // shifting the elements in the vector
    m_usageData[0] = load;
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());
    return load;
}
std::string CPUMeasure::getUptimeStr() const {
    const auto uptimeS = (m_uptime / 1000) % 60;
    const auto uptimeM = (m_uptime / (60 * 1000)) % 60;
    const auto uptimeH = (m_uptime / (1000 * 60 * 60)) % 24;
    const auto uptimeD = (m_uptime / (1000 * 60 * 60 * 24));

    std::stringstream formatStream;
    // Format so each number is 2 digits with leading 0s when necessary
    formatStream << std::setfill('0') << std::setw(2) << uptimeD.count() << ":"
                 << std::setw(2) << uptimeH.count() << ":"
                 << std::setw(2) << uptimeM.count() << ":"
                 << std::setw(2) << uptimeS.count();

    return formatStream.str();
}

float CPUMeasure::calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks) {
    /* Credit to Jeremy Friesner
       http://stackoverflow.com/questions/23143693/retrieving-cpu-load-percent-total-in-windows-with-c */
    static uint64_t prevTotalTicks{ 0U };
    static uint64_t prevIdleTicks{ 0U };

    const uint64_t totalTicksSinceLastTime{ totalTicks - prevTotalTicks };
    const uint64_t idleTicksSinceLastTime{ idleTicks - prevIdleTicks };

    const float cpuLoad{ 1.0f - ((totalTicksSinceLastTime > 0) ?
                     (static_cast<float>(idleTicksSinceLastTime)) / totalTicksSinceLastTime : 0) };

    prevTotalTicks = totalTicks;
    prevIdleTicks = idleTicks;

    return cpuLoad;
}

}

