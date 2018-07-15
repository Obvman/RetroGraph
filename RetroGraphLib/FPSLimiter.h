#pragma once

#include "stdafx.h"

#include <cstdint>

namespace rg {

class FontManager;

class FPSLimiter {
public:
    static FPSLimiter& inst() { static FPSLimiter i; return i; }

    ~FPSLimiter() noexcept = default;
    FPSLimiter(const FPSLimiter&) = delete;
    FPSLimiter& operator=(const FPSLimiter&) = delete;
    FPSLimiter(FPSLimiter&&) = delete;
    FPSLimiter& operator=(FPSLimiter&&) = delete;

    /* Call at the beginning of the frame */
    void begin();

    /* Called at the end of the frame to calculate frame time */
    void end();

    float getFPS() const { return m_fps;  }

    void setMaxFPS(int maxFPS) { m_maxFPS = maxFPS; }
private:
    FPSLimiter();

    /* Calculates the framerate using multiple previous frame times to smooth 
     * the value 
     */
    void calculateFPS();

    float m_fps;
    int m_maxFPS;
    float m_frameTime;
    int64_t m_startTicks;
    int64_t m_freq;
};

} // namespace rg
