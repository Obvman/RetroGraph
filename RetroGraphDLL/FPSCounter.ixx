export module FPSCounter;

import std.core;

namespace rg {

export class FPSCounter {
public:
    FPSCounter();
    ~FPSCounter() noexcept = default;
    FPSCounter(const FPSCounter&) = delete;
    FPSCounter& operator=(const FPSCounter&) = delete;
    FPSCounter(FPSCounter&&) = delete;
    FPSCounter& operator=(FPSCounter&&) = delete;

    /* Call at the beginning of the frame */
    __declspec(dllexport) void startFrame();

    /* Called at the end of the frame to calculate frame time */
    __declspec(dllexport) void endFrame();

    float getFPS() const { return m_fps; }
private:
    /* Calculates the framerate using multiple previous frame times to smooth 
     * the value 
     */
    void calculateFPS();

    float m_fps;
    float m_frameTime;
    int64_t m_startTicks;
    int64_t m_freq;

    static constexpr int numFrameTimeSamples{ 15 };
    std::array<float, numFrameTimeSamples> m_frameTimeHistory;
    int m_currFrameIdx;
};

} // namespace rg
