module FPSLimiter;

import Core.Time;

namespace rg {

using namespace std::chrono;

constexpr microseconds fpsToFrameTime(int fps) {
    return duration_cast<microseconds>(duration<double>{ 1 } / fps);
}

FPSLimiter::FPSLimiter(int fps)
    : m_frameTime{ fpsToFrameTime(fps) }
    , m_currentFrameStart{ system_clock::now() }
    , m_currentFrameEnd{ m_currentFrameStart + m_frameTime } {

}

void FPSLimiter::setFPS(int fps) {
    m_frameTime = fpsToFrameTime(fps);
}

void FPSLimiter::startFrame() {
    const auto now = system_clock::now();
    //std::cerr << "This frame: " << round<milliseconds>(now - m_currentFrameStart) << '\n';
    m_currentFrameStart = now;
}

void FPSLimiter::endFrame() {
    const auto now = system_clock::now();

    sleepUntil(m_currentFrameEnd);

    // Handle large jumps in time (e.g. pausing the debugger or putting computer to sleep)
    if (m_currentFrameEnd < now)
        m_currentFrameEnd = now;

    m_currentFrameEnd += m_frameTime;
}

} // namespace rg
