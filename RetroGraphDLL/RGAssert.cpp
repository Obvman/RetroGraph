import std.core;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

void showMessageBox(const char * s) {
    MessageBox(nullptr, s, "Error", MB_OK | MB_ICONERROR);
}

void rgAssert(bool expr, const char* str) {
    if (!expr)
        showMessageBox(str);
}

void rgError(const char* str) {
    showMessageBox(str);
}

void rgVerify([[maybe_unused]] bool expr, [[maybe_unused]] const char* str) {
    if constexpr (debugMode)
        if (!expr)
            showMessageBox(str);
}

}
