import FPSLimiter;
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

    rg::FPSLimiter fpsLimiter;

    // Enter main update/draw loop
    while (retroGraph.isRunning()) {
        // Handle Windows messages
        MSG msg{};
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        retroGraph.update();
        retroGraph.draw();

        // Lay off the CPU a little
        fpsLimiter.endFrame();

        retroGraph.getFPSCounter().endFrame();
        retroGraph.getFPSCounter().startFrame();
        fpsLimiter.startFrame();
    }
}

