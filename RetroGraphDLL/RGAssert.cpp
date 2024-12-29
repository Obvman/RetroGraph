import std;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

void showMessageBox(const char* s) {
    MessageBox(nullptr, s, "Error", MB_OK | MB_ICONERROR);
}

void showAssertMessageBox(const char* s) {
    const auto result{ MessageBox(nullptr, s, "Assert", MB_ABORTRETRYIGNORE | MB_ICONERROR) };
    switch (result) {
        case IDIGNORE:
            break;
        case IDABORT:
            std::abort();
        case IDRETRY:
            DebugBreak();
            break;
    }
}

void rgAssert(bool expr, const char* str) {
    if (!expr)
        showAssertMessageBox(str);
}

void rgError(const char* str) {
    showAssertMessageBox(str);
}

void rgVerify([[maybe_unused]] bool expr, [[maybe_unused]] const char* str) {
    if constexpr (debugMode)
        if (!expr)
            showAssertMessageBox(str);
}

} // namespace rg
