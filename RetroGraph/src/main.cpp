#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>

#include <GL/glew.h>
#include <GL/gl.h>
#include <tweeny/tweeny.h>

#include <Windows.h>
//#include <winternl.h>
//#include <comdef.h>
//#include <Wbemidl.h>
//#pragma comment(lib, "wbemuuid.lib")
//#pragma comment(lib, "Ntdll.lib")


#include "../headers/utils.h"
#include "../headers/RetroGraph.h"

void mainLoop(rg::RetroGraph& retroGraph);

#if _DEBUG
int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
#else
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    hInstance = GetModuleHandle(nullptr);
#endif

    try {
        rg::RetroGraph retroGraph{ hInstance };

        mainLoop(retroGraph);
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}

void mainLoop(rg::RetroGraph& retroGraph) {
    using namespace std::chrono;

    auto frameStartTime{ duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        ).count() };

    auto ticks = uint32_t{ 1 };
    auto lastTick{ ticks };

    // Enter main update/draw loop
    MSG msg;
    while(retroGraph.isRunning()) {
        const auto currTime{ duration_cast<milliseconds>(
                                  system_clock::now().time_since_epoch()
                             ).count() };
        const auto dt{ currTime - frameStartTime };

        // Handle Windows messages
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Execute timed actions when the tick rolls over
        if (lastTick != ticks) {
            retroGraph.update(ticks);
            retroGraph.draw(ticks);

            lastTick = ticks;
        }

        // Reset the millisecond counter every tick, and roll over to next tick
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
        Sleep(8);
    }
}

