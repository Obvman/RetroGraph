import RG.Application;

import std.core;

import "../RetroGraphDLL/RGAssert.h";
import "../RetroGraphDLL/WindowsHeaderUnit.h";

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
        retroGraph.run();

    } catch (const std::runtime_error& e) {
        std::cout << e.what() << '\n';
        RGERROR(("Unhandled application exception: " + std::string{ e.what() }).c_str());
    }

    return 0;
}
