#pragma once

#include <cstdint>

namespace rg {

class FontManager;

class FPSLimiter {
public:
    static FPSLimiter& inst() {
        static FPSLimiter i;
        return i;
    }

    ~FPSLimiter() noexcept = default;
    FPSLimiter(const FPSLimiter&) = delete;
    FPSLimiter& operator=(const FPSLimiter&) = delete;
    FPSLimiter(FPSLimiter&&) = delete;
    FPSLimiter& operator=(FPSLimiter&&) = delete;

    void begin();
    void end();

    float getFPS() const { return m_fps;  }
private:
    FPSLimiter();
    void calculateFPS();

    float m_fps;
    uint32_t m_maxFPS;
    float m_frameTime;
    int64_t m_startTicks;
    int64_t m_freq;
};

} // namespace rg
