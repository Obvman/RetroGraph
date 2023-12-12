export module FPSLimiter;

import UserSettings;

import std.core;

namespace rg {

export class FPSLimiter {
public:
    __declspec(dllexport) FPSLimiter();
    __declspec(dllexport) ~FPSLimiter();

    __declspec(dllexport) void startFrame();
    __declspec(dllexport) void endFrame();

private:
    ConfigRefreshedCallbackHandle RegisterConfigRefreshedCallback();

    std::chrono::microseconds m_frameTime;
    std::chrono::system_clock::time_point m_currentFrameStart;
    std::chrono::system_clock::time_point m_currentFrameEnd;
    ConfigRefreshedCallbackHandle m_configRefreshedHandle;
};

} // namespace rg
