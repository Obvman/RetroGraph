#include "../headers/CPUMeasure.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <time.h>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "../headers/Window.h"
#include "../headers/colors.h"
#include "../headers/utils.h"

static unsigned long long FileTimeToInt64(const FILETIME & ft) {return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);}

namespace rg {

CPUMeasure::CPUMeasure(int32_t graphWidth, int32_t graphHeight) :
    m_coreTempPlugin{},
    dataSize{ 40U },
    m_usageData{ },
    m_uptime{ std::chrono::milliseconds{GetTickCount64()} },
    m_cpuName{ },
    m_viewportStartX{ 0 },
    m_viewportStartY{ 480 },
    m_viewportWidth{ graphWidth },
    m_viewportHeight{ graphHeight } {

    // fill vector with default values
    m_usageData.assign(dataSize, 0.5f);
}

CPUMeasure::~CPUMeasure() {
}

void CPUMeasure::update() {
    m_coreTempPlugin.update();
    m_uptime = std::chrono::milliseconds(GetTickCount64());

    // Fill CPU name if CoreTemp interfacing was successful
    if (m_cpuName.size() == 0 && m_coreTempPlugin.getCoreTempInfoSuccess()) {
        m_cpuName = "CPU: ";
        m_cpuName.append(m_coreTempPlugin.getCPUName());
    }

    getCPULoad();
}

void CPUMeasure::drawText() const {
    const auto numCores{ m_coreTempPlugin.getNumCores() };
    const auto numLines = uint32_t{ 3U + numCores };
    constexpr auto rasterX = float{ -0.95f };
    auto rasterY = float{ -0.95f };

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    // Set viewport for the graph to left half of measure's viewport
    glViewport(0, m_viewportStartY, m_viewportWidth/2, m_viewportHeight);

    // Draw voltage
    const std::string vStr{ "Voltage: " + std::to_string(m_coreTempPlugin.getVoltage()) + "v"};
    glRasterPos2f(rasterX, rasterY);
    glCallLists(vStr.size(), GL_UNSIGNED_BYTE, vStr.c_str());
    rasterY += 2.0f / numLines;

    // Draw clock speed
    const std::string clockStr{ "Clock: " + std::to_string(m_coreTempPlugin.getClockSpeed()) + "MHz"};
    glRasterPos2f(rasterX, rasterY);
    glCallLists(clockStr.size(), GL_UNSIGNED_BYTE, clockStr.c_str());
    rasterY += 2.0f / numLines;

    // Draw average core temp
    auto maxTemp = uint32_t{ 0 };
    for (auto i{ 0U }; i < numCores; ++i) {
        if (m_coreTempPlugin.getTemp(i) > maxTemp) {
            maxTemp = static_cast<uint32_t>(m_coreTempPlugin.getTemp(i));
        }
    }

    const std::string tempStr{ "CPU Temp: " + std::to_string(maxTemp) + "C"};
    glRasterPos2f(rasterX, rasterY);
    glCallLists(tempStr.size(), GL_UNSIGNED_BYTE, tempStr.c_str());
    rasterY += 2.0f / numLines;

    // Draw usage % and bar for each core
    for (auto i{ 0U }; i < numCores; ++i) {
        const auto usage { m_coreTempPlugin.getLoad(i) };

        const std::string usageStr{ "Core " + std::to_string(i) + ": " +
                                    std::to_string(usage) + "%" };
        glRasterPos2f(rasterX, rasterY);
        glCallLists(usageStr.size(), GL_UNSIGNED_BYTE, usageStr.c_str());
        rasterY += 2.0f / numLines;

        // Draw a usage meter bar
        const auto percentFilled = float{ (usage / 100.0f) * 2.0f };
        GLfloat lineWidth;
        glGetFloatv(GL_LINE_WIDTH, &lineWidth);

        float color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);

        glColor3f(LINE_R, LINE_G, LINE_B);
        glLineWidth(5.0f);

        const auto drawY{ rasterY - 0.3f };
        glBegin(GL_LINES); {
            glVertex2f(-0.95f, drawY);
            glVertex2f(-0.95f + percentFilled, drawY);

            // Draw available section
            glColor3f(0.2f, 0.2f, 0.2f);
            glVertex2f(-0.95f + percentFilled, drawY);
            glVertex2f(0.95f, drawY);
        } glEnd();

        glLineWidth(lineWidth);
        glColor4f(color[0], color[1], color[2], color[3]);
    }
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

