export module FPSLimiter;

import RG.Application;

import std.core;

namespace rg {

export class FPSLimiter {
public:
    __declspec(dllexport) FPSLimiter();
    __declspec(dllexport) ~FPSLimiter();

    __declspec(dllexport) void startFrame();
    __declspec(dllexport) void endFrame();

private:
    ConfigRefreshedEvent::Handle RegisterConfigRefreshedCallback();

    std::chrono::microseconds m_frameTime;
    std::chrono::system_clock::time_point m_currentFrameStart;
    std::chrono::system_clock::time_point m_currentFrameEnd;
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
};

} // namespace rg
