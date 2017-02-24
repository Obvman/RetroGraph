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

}
