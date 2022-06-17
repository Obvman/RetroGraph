#include "RGAssert.h"

import <stdexcept>;

import <WindowsHeaders.h>;

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

void rgVerify(bool expr [[maybe_unused]], const char* str[[maybe_unused]]) {
    if constexpr (debugMode)
        if (!expr)
            showMessageBox(str);
}

}
