#include <iostream> // std
#include <iomanip>
#include <chrono>

#include <Windows.h> // win32
#include <TlHelp32.h>
#include <Psapi.h>

#include <GL/glew.h> // openGL
#include <GL/gl.h>

#include "CPUMeasure.h"
#include "Window.h"

void test();

//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    rg::Window mainWindow{ hInstance, "RetroGraph" };

    using namespace std::chrono;
    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    constexpr uint16_t maxTicks{ 10U };
    constexpr uint32_t tickDuration{ 100U }; // tick length in milliseconds
    uint16_t ticks{ 1 };
    auto lastTick = ticks;
    MSG msg;

    // Perform an update/draw call before entering the loop so the window shows
    // immediately at startup instead of after a few ticks
    mainWindow.init();

    //test();

    // Enter main update/draw loop
    while(true) {
        const auto currTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        auto dt = currTime - start;

        // Handle windows messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Execute timed actions when the tick rolls over
        if (lastTick != ticks) {
            mainWindow.update(ticks);

            // Draw every 5 ticks
            if (lastTick % 5 == 0) {
                mainWindow.draw();
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

void test() {
}