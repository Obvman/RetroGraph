#include <iostream>
#include <iomanip>
#include <chrono>

#include <Windows.h>
#include <winternl.h>
#include <Wbemidl.h>
#include <comdef.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../headers/utils.h"
#include "../headers/CPUMeasure.h"
#include "../headers/Window.h"

void mainLoop(rg::Window& mainWindow);

#if (!_DEBUG)
// Release mode is a Win32 application, while Debug mode is a console application
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInstance = GetModuleHandle(nullptr);
#endif

#if _DEBUG
int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
#endif

    rg::Window mainWindow{ hInstance, "RetroGraph", 1920U, 1170U, 2560, 0 };

    // Perform an update/draw call before entering the loop so the window shows
    // immediately at startup instead of after a few ticks
    mainWindow.init();

    mainLoop(mainWindow);

    return 0;
}

void mainLoop(rg::Window& mainWindow) {
    using namespace std::chrono;
    auto start{ duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() };
    uint32_t ticks{ 1 };
    auto lastTick{ ticks };
    MSG msg;

    // Enter main update/draw loop
    while(true) {
        const auto currTime{ duration_cast<milliseconds>(
                                  system_clock::now().time_since_epoch()
                              ).count() };
        const auto dt{ currTime - start };

        // Handle windows messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Execute timed actions when the tick rolls over
        if (lastTick != ticks) {
            mainWindow.update(ticks);

            // Draw every half second
            if ((lastTick % (rg::ticksPerSecond/2)) == 0) {
                mainWindow.draw();
            }

            lastTick = ticks;
        }

        // Reset the millisecond counter every tick
        if (dt > rg::tickDuration) {
            start = currTime;
            ++ticks;
        }

        // Keep the ticks counter range 1 - maxTicks to prevent overflow
        if (ticks > rg::maxTicks) {
            ticks = 1U;
        }

        // Lay off the CPU a little
        Sleep(15);
    }
}
