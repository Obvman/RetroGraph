export module FPSLimiter;

import std.core;

namespace rg {

export class FPSLimiter {
public:
    __declspec(dllexport) FPSLimiter(int fps);

    __declspec(dllexport) void setFPS(int fps);

    __declspec(dllexport) void startFrame();
    __declspec(dllexport) void endFrame();

private:
    std::chrono::microseconds m_frameTime;
    std::chrono::system_clock::time_point m_currentFrameStart;
    std::chrono::system_clock::time_point m_currentFrameEnd;
};

} // namespace rg
