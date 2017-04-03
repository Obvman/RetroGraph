#include "../headers/utils.h"

#include <GL/glew.h>
#include <locale>
#include <codecvt>

#include "../headers/colors.h"

namespace rg {

void showMessageBox(const std::string& s) {
    MessageBox(nullptr, s.c_str(), "Error", MB_OK | MB_ICONERROR);
}

void fatalMessageBox(const std::string& s) {
    showMessageBox(s);
    throw std::runtime_error(s);
}

std::string wstrToStr(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

std::wstring strToWstr(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
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


constexpr float lerp(float x1, float x2, float t) {
    return (1 - t) * x1 + t * x2;
}

}
