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
    std::wstring_convert<std::codecvt_utf16<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

std::wstring strToWstr(const std::string& str) {
    std::wstring_convert<std::codecvt_utf16<wchar_t>, wchar_t> converter;
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

void drawViewportBorder() {
    float color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);

    glColor3f(DEBUG_BORDER_R, DEBUG_BORDER_G, DEBUG_BORDER_B);

    // Preserve initial line width
    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    glLineWidth(5.0f);

    glBegin(GL_LINES); {
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f,  1.0f);

        glVertex2f(-1.0f,  1.0f);
        glVertex2f( 1.0f,  1.0f);

        glVertex2f( 1.0f,  1.0f);
        glVertex2f( 1.0f, -1.0f);

        glVertex2f( 1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
    } glEnd();

    glLineWidth(lineWidth);

    glColor4f(color[0], color[1], color[2], color[3]);
    glLineWidth(lineWidth);
}

float lerp(float x1, float x2, float t) {
    //return x1 + (x2 - x1) * t;
    return (1 - t) * x1 + t * x2;
}

}
