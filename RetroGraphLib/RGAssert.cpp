#include "RGAssert.h"

import <stdexcept>;

import <WindowsHeaders.h>;

namespace rg {

//void showMessageBox(const std::string& /*s*/) {
//    rg::AssertDialog dlg;
//    //dlg.DoModal();
//}

void showMessageBox(const char * s) {
    MessageBox(nullptr, s, "Error", MB_OK | MB_ICONERROR);
}

/* Displays an Error message box with the given string as a message and
   exits the program with a failure code */
void fatalMessageBox(const char* s) {
    showMessageBox(s);
    if constexpr (debugMode)
        DebugBreak();
    throw std::runtime_error(s);
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
