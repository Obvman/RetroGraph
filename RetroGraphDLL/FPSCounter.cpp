module FPSCounter;

import UserSettings;

import "WindowsHeaderUnit.h";

namespace rg {

FPSCounter::FPSCounter()
    : m_fps{ 0 }
    , m_frameTime{ 0.0 }
    , m_startTicks{ 0 }
    , m_freq{ 0 } {

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_freq));
}

void FPSCounter::startFrame() {
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTicks));
}

void FPSCounter::endFrame() {
    calculateFPS();
}

void FPSCounter::calculateFPS() {
    constexpr int NUM_SAMPLES = 15;

    // #TODO static variables into members
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

} // namespace rg
