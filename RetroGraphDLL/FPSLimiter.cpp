module FPSLimiter;

import UserSettings;

import RG.Core;

namespace rg {

using namespace std::chrono;

constexpr microseconds fpsToFrameTime(int fps) {
    return duration_cast<microseconds>(duration<double>{ 1 } / fps);
}

FPSLimiter::FPSLimiter()
    : m_frameTime{ fpsToFrameTime(UserSettings::inst().getVal<int>("Application.FPS")) }
    , m_currentFrameStart{ system_clock::now() }
    , m_currentFrameEnd{ m_currentFrameStart + m_frameTime }
    , m_configRefreshedHandle{ RegisterConfigRefreshedCallback() } {

}

FPSLimiter::~FPSLimiter() {
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
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

ConfigRefreshedEvent::Handle FPSLimiter::RegisterConfigRefreshedCallback() {
    return UserSettings::inst().configRefreshed.attach(
        [this]() {
            m_frameTime = fpsToFrameTime(UserSettings::inst().getVal<int>("Application.FPS"));
        }
    );
}

} // namespace rg
