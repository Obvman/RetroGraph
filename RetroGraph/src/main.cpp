#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>

#include <Windows.h>
#include <winternl.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Ntdll.lib")

#include <GL/glew.h>
#include <GL/gl.h>

#include "../headers/utils.h"
#include "../headers/CPUMeasure.h"
#include "../headers/Window.h"

void mainLoop(rg::Window& mainWindow);

#if _DEBUG
int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
#else
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    hInstance = GetModuleHandle(nullptr);
#endif
    try {
        rg::Window mainWindow{ hInstance };
        mainWindow.init();

        mainLoop(mainWindow);
    } catch(const std::runtime_error& e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}

void mainLoop(rg::Window& mainWindow) {
    using namespace std::chrono;

    auto frameStartTime{ duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count()
    };
    auto ticks = uint32_t{ 1 };
    auto lastTick{ ticks };
    constexpr auto framesPerSecond = uint32_t{ 2U };

    // Enter main update/draw loop
    MSG msg;
    while(true) {
        const auto currTime{ duration_cast<milliseconds>(
                                  system_clock::now().time_since_epoch()
                             ).count() };
        const auto dt{ currTime - frameStartTime };

        // Handle windows messages
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Execute timed actions when the tick rolls over
        if (lastTick != ticks) {
            /* rg::printTimeToExecuteHighRes("Update", [&]() { */
            mainWindow.update(ticks);
            /* }); */

            // Draw according to the framerate
            if ((lastTick % std::lround(
                static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

                /* rg::printTimeToExecuteHighRes("Draw", [&]() { */
                mainWindow.draw(ticks);
                /* }); */
            }

            lastTick = ticks;
        }

        // Reset the millisecond counter every tick
        if (dt > rg::tickDuration) {
            frameStartTime = currTime;
            ++ticks;
        }

        // Keep the ticks counter range 1 - maxTicks to prevent overflow
        if (ticks > rg::maxTicks) {
            ticks = 1U;
        }

        // Force flush to stdout for Cygwin
        #if _DEBUG
            fflush(stdout);
        #endif

        // Lay off the CPU a little
        Sleep(15);
    }
}

