#pragma once

#include <GL/glew.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <chrono>

namespace rg {

class CPUMeasure {
public:
    CPUMeasure(int32_t graphWidth, int32_t graphHeight);
    ~CPUMeasure();

    void update();
    void draw() const;
    void drawUptime() const;
    void drawGraphBox() const;
    float getCPULoad();

    std::string getUptimeStr() const;
private:
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);

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