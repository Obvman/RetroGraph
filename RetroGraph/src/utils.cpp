#include "../headers/utils.h"

#include <GL/glew.h>

#include "../headers/colors.h"

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

}
