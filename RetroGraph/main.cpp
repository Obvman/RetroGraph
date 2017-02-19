#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <iomanip>
#include <chrono>

#include "CPUMeasure.h"
#include "Window.h"

int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    rg::CPUMeasure cpuMeasure{};
    rg::Window mainWindow{ hInstance, "RetroGraph" };

    mainWindow.draw(cpuMeasure.m_usageData);

    using namespace std::chrono;
    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint16_t ticks{ 1 };
    auto lastTick = ticks;
    constexpr uint16_t maxTicks{ 10U };
    constexpr uint32_t tickDuration{ 100U };
    MSG msg;

    // Enter main update/draw loop
    while(true) {
        const auto currTime = duration_cast<milliseconds>(
                                  system_clock::now().time_since_epoch()
                              ).count();
        auto dt = currTime - start;

        // Handle windows messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Execute timed actions when the tick rolls over
        if (lastTick != ticks) {

            // Draw every 10 ticks
            if (lastTick % 10 == 0) {
                mainWindow.draw(cpuMeasure.m_usageData);
            }

            // update CPU usage tracker every 5 ticks
            if (lastTick % 5 == 0) {
                auto cpuLoad = cpuMeasure.getCPULoad();
                std::cout << std::setprecision(3) << "CPU Usage: " << cpuLoad * 100.0f << "%\n";

                //std::cout << "Vec size: " << cpuMeasure.m_usageData.size() << '\n';
                //for (const auto i : cpuMeasure.m_usageData) {
                    //std::cout << i << ' ';
                //}
                //std::cout << '\n';
            }

            lastTick = ticks;
        }


        // Reset the counter every tick - the smallest expected interval for
        // an action
        if (dt > tickDuration) {
            start = currTime;
            ++ticks;
        }

        // Keep the ticks counter range 1 - maxTicks
        if (ticks > maxTicks) {
            ticks = 1U;
        }

        // Lay off the CPU a little
        Sleep(10);
    }
    return 0;
}