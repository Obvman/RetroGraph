module FPSCounter;

import "WindowsHeaderUnit.h";

namespace rg {

FPSCounter::FPSCounter()
    : m_fps{ 0 }
    , m_frameTime{ 0.0 }
    , m_startTicks{ 0 }
    , m_freq{ 0 }
    , m_frameTimeHistory{ 0.0f }
    , m_currFrameIdx{ 0 } {

    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_freq));
}

void FPSCounter::startFrame() {
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_startTicks));
}

void FPSCounter::endFrame() {
    calculateFPS();
}

void FPSCounter::calculateFPS() {
    int64_t endTicks;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&endTicks));

    m_frameTime = static_cast<float>(endTicks - m_startTicks) / m_freq;
    m_frameTimeHistory[m_currFrameIdx % numFrameTimeSamples] = m_frameTime;

    int count{ numFrameTimeSamples };
    ++m_currFrameIdx;
    if (m_currFrameIdx < numFrameTimeSamples) {
        count = m_currFrameIdx;
    }

    float frameTimeAverage{ 0.0f };
    for (int i = 0; i < count; ++i) {
        frameTimeAverage += m_frameTimeHistory[i];
    }
    frameTimeAverage /= count;

    if (frameTimeAverage > 0) {
        m_fps = 1.0f / frameTimeAverage;
    } else {
        m_fps = 0.0f;
    }
}

} // namespace rg
