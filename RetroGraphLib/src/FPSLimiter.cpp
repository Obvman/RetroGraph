#include "../headers/FPSLimiter.h"

#include <Windows.h>
#include <thread>
#include <chrono>

#include "../headers/UserSettings.h"

namespace rg {

/*BOOLEAN nanosleep(LONGLONG ns){
    HANDLE timer;
    LARGE_INTEGER li;
    if(!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
        return FALSE;
    li.QuadPart = -ns;
    if(!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)){
        CloseHandle(timer);
        return FALSE;
    }
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
    return TRUE;
}*/

/*static void sleep_for(double dt) {
    using clock = std::chrono::high_resolution_clock;

    static constexpr std::chrono::duration<double> MinSleepDuration(0);
    clock::time_point start = clock::now();
    while (std::chrono::duration<double>(clock::now() - start).count() < dt) {
        std::this_thread::sleep_for(MinSleepDuration);
    }
}*/

FPSLimiter::FPSLimiter() : 
    m_maxFPS{ std::get<uint32_t>(
        UserSettings::inst().getVal("Widgets-Main.FPS")) } {
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
}


void FPSLimiter::begin() {
    QueryPerformanceCounter((LARGE_INTEGER*)&m_startTicks);
}

void FPSLimiter::end() {

    calculateFPS();

    // int frameTicks = SDL_GetTicks() - m_startTicks;

    /*uint64_t frameTicks;
    QueryPerformanceCounter((LARGE_INTEGER*)&frameTicks);
    frameTicks -= m_startTicks;
    // printf("seconds over target: %f\n", static_cast<float>(frameTicks) / m_freq);

    printf("Frametime: %f\n", m_frameTime);
    printf("Target frametime: %f\n", 1.0f / m_maxFPS);

    const auto targetFrameTime{ 1.0f / static_cast<float>(m_maxFPS) };

    // If the frametime is higher than it should be to meet max fps, 
    // sleep the extra cycles
    if (m_frameTime < targetFrameTime) {
        sleep_for(1*(targetFrameTime - m_frameTime));
        const auto toSleep{ targetFrameTime - m_frameTime };
        uint64_t toSleepNs{ static_cast<uint64_t>(toSleep * 1000000000) };

        printf("Sleeping for %f seconds, %lld ns\n", toSleep, toSleepNs);

        nanosleep(toSleepNs);

    }*/

    // limit FPS
    // if (1000.0f / m_maxFPS > frameTicks) {
        // TODO
        // SDL_Delay(int(1000.0f / m_maxFPS - frameTicks));
    // }
}

void FPSLimiter::calculateFPS() {
    static const int NUM_SAMPLES = 4;
    static float frameTimes[NUM_SAMPLES];
    static int currFrame = 0;

    // static int prevTicks = SDL_GetTicks();
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
