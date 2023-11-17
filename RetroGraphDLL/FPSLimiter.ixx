export module FPSLimiter;

import UserSettings;

import "WindowsHeaderUnit.h";

namespace rg {

export { class FPSLimiter; }

SettingVariant variant = {};

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

FPSLimiter::FPSLimiter()
    : m_maxFPS{ UserSettings::inst().getVal<int>("Widgets-Main.FPS") }
    , m_fps{ 0 }
    , m_frameTime{ 0.0 }
    , m_startTicks{ 0 }
    , m_freq{ 0 } {

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_freq));
}


void FPSLimiter::begin() {
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTicks));
}

void FPSLimiter::end() {
    calculateFPS();
}

void FPSLimiter::calculateFPS() {
    static const int NUM_SAMPLES = 4;
    static float frameTimes[NUM_SAMPLES];
    static int currFrame = 0;

    int64_t currTicks;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTicks);
    static int64_t prevTicks{ currTicks };

    m_frameTime = static_cast<float>(currTicks - prevTicks) / m_freq;
    frameTimes[currFrame % NUM_SAMPLES] = m_frameTime;
    prevTicks = currTicks;

    int count;
    ++currFrame;
    if (currFrame < NUM_SAMPLES) {
        count = currFrame;
    } else {
        count = NUM_SAMPLES;
    }

    float frameTimeAverage = 0;
    for (int i = 0; i < count; ++i) {
        frameTimeAverage += frameTimes[i];
    }
    frameTimeAverage /= count;

    if (frameTimeAverage > 0) {
        m_fps = 1.0f / frameTimeAverage;
    } else {
        m_fps = 0.0f;
    }
}

}
