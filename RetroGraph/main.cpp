import IRetroGraph;
import RetroGraph;
import Utils;
import Units;

import std.core;

import "../RetroGraphDLL/RGAssert.h";
import "../RetroGraphDLL/WindowsHeaderUnit.h";

void mainLoop(rg::IRetroGraph& retroGraph);

// Allows debug mode to show debug console, and release mode hides it
#if _DEBUG
int main() {
    HINSTANCE hInstance{ GetModuleHandle(nullptr) };
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

void mainLoop(rg::IRetroGraph& retroGraph) {
    using namespace std::chrono;

    auto frameStartTime{ duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        ).count() };

    auto ticks = int{ 1 };
    auto lastTick{ ticks };

    // Enter main update/draw loop
    while (retroGraph.isRunning()) {

        const auto currTime{ duration_cast<milliseconds>(
                                  system_clock::now().time_since_epoch()
                             ).count() };
        const auto dt{ currTime - frameStartTime };

        // Handle Windows messages
        MSG msg{};
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
        if (dt >= rg::tickDuration) {
            frameStartTime = currTime;
            ++ticks;
        }

        // Keep the ticks counter in range [1, maxTicks] to prevent overflow
        if (ticks > rg::maxTicks)
            ticks = 1;

        // Lay off the CPU a little
        Sleep(2);
    }
}

