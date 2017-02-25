#pragma once

#include <GL/glew.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <chrono>

namespace rg {

class Window;

class CPUMeasure {
public:
    CPUMeasure(Window* w, int32_t graphWidth, int32_t graphHeight);
    ~CPUMeasure();

    /* Updates the total system's CPU usage statistics */
    void update();

    /* Draws the components of the measure */
    void draw(GLuint shader) const;


    /* Returns the current system CPU load as a percentage */
    float getCPULoad();

private:
    /* Draws the line graph of system CPU usage */
    void drawGraph(GLuint shader) const;

    /* Draws the total system uptime text */
    void drawUptime() const;

    /* Draws the system time in hh:mm:ss format */
    void drawSystemTime() const;

    /* Returns string containing the current system uptime in
       dd:hh:mm:ss format */
    std::string getUptimeStr() const;

    /* Calculates the total CPU load with the given tick information */
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);

    Window* m_parentWindow;
    size_t dataSize; // max number of usage percentages to store
    std::vector<float> m_usageData;
    std::chrono::milliseconds m_uptime;

    // Rendering members
    GLint m_viewportStartX;
    GLint m_viewportStartY;
    GLint m_viewportWidth;
    GLint m_viewportHeight;
};

}