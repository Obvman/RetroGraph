#include "stdafx.h"

#include "FPSLimiter.h"

#include <Windows.h>
#include <thread>
#include <chrono>

#include "UserSettings.h"

namespace rg {

FPSLimiter::FPSLimiter()
    : m_maxFPS{ UserSettings::inst().getVal<int>("Widgets-Main.FPS") } {

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
