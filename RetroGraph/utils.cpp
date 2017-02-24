#include "utils.h"

namespace rg {

void showMessageBox(const std::string& s) {
    MessageBox(nullptr, s.c_str(), "Error", MB_OK | MB_ICONERROR);
}

void fatalMessageBox(const std::string& s) {
    showMessageBox(s);
    exit(1);
}

ULARGE_INTEGER ftToULI(const FILETIME& ft) {
     ULARGE_INTEGER uli;
     uli.LowPart = ft.dwLowDateTime;
     uli.HighPart = ft.dwHighDateTime;

     return uli;
}

uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB) {
     LARGE_INTEGER a, b;
     a.LowPart = ftA.dwLowDateTime;
     a.HighPart = ftA.dwHighDateTime;

     b.LowPart = ftB.dwLowDateTime;
     b.HighPart = ftB.dwHighDateTime;

     return a.QuadPart - b.QuadPart;
}

}
