#include "Window.h"
#include <GL/gl.h>
#include <dwmapi.h>
#include <iostream>

namespace rg {

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Do some silly casting magic so this function can access Window's members
    // using the pThis pointer
    Window* pThis{ nullptr };
    if (msg == WM_CREATE) {
        pThis = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(pThis))) {
            if (GetLastError() != 0)
                return false;
        }
    } else {
        pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWL_USERDATA));
    }

    static PAINTSTRUCT ps;
    switch (msg) {
        case WM_PAINT:
            /*std::cout << "Painting\n";
            pThis->draw();
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);*/
            return 0;
        case WM_SIZE:
            std::cout << "WM_SIZE\n";
            glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
            PostMessage(hWnd, WM_PAINT, 0, 0);
            return 0;
        case WM_CREATE:
            std::cout << "WM_CREATE\n";
            break;
        case WM_SETCURSOR:
            std::cout << "Setting cursor\n";
            break;
        case WM_NCHITTEST:
            break;
        case WM_MOUSEMOVE:
            std::cout << "WM_MOUSEMOVE\n";
            break;
        case WM_GETTEXT:
            break;
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
            std::cout << "WM_DESTROY\n";
            pThis->releaseOpenGL();
            PostQuitMessage(0);
            return 0;
        default:
            //std::cout << "Handling message code: " << msg << '\n';
            break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(HINSTANCE hInstance, const char* windowName) {
    WNDCLASSEX m_wc;
    memset(&m_wc, 0, sizeof(m_wc));
    m_wc.cbSize = sizeof(WNDCLASSEX);
    m_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    m_wc.style = CS_HREDRAW | CS_VREDRAW;
    m_wc.lpfnWndProc = Window::WndProc;
    m_wc.cbClsExtra  = 0;
    m_wc.cbWndExtra  = 0;
    m_wc.hInstance = hInstance;
    m_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    m_wc.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
    m_wc.lpszClassName = windowName;

    if(!RegisterClassEx(&m_wc)) {
        MessageBox(NULL, "RegisterClassEx - failed", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, "RetroGraph", "RetroGraph",
                               WS_VISIBLE | WS_POPUP, 200, 150, 240, 240,
                               NULL, NULL, hInstance, NULL);

    if(!m_hWnd) {
        MessageBox(NULL, "CreateWindowEx - failed", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    DWM_BLURBEHIND bb = {0};
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(m_hWnd, &bb);

    createHGLRC();

    m_hdc = GetDC(m_hWnd);
    wglMakeCurrent(m_hdc, m_hrc);
    initOpenGL();
    updateSize(240, 240);

    ReleaseDC(m_hWnd, m_hdc);
}


Window::~Window() {
}

void Window::show() {
    ShowWindow(m_hWnd, SW_SHOW);
}

void Window::draw(const std::vector<float>& floats) const {
    HDC hdc = GetDC(m_hWnd);
    wglMakeCurrent(hdc, m_hrc);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    //glClearColor(1.0f, 0.0f, 0.5f, 0.001f);

    glBegin(GL_LINES);

    for (auto i{ 0U }; i < floats.size() - 1; ++i) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(i / 10.0f, floats[i], 0.0f);
        glVertex3f((i + 1)/ 10.0f, floats[i + 1], 0.0f);
    }

    glEnd();


    SwapBuffers(hdc);
    ReleaseDC(m_hWnd, hdc);
}

void Window::updateSize(int width, int height) {
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW );
    glLoadIdentity();
}

void Window::initOpenGL() const {
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Window::releaseOpenGL() {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_hrc);
}

bool Window::createHGLRC() {

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                                // Version Number
        PFD_DRAW_TO_WINDOW      |         // Format Must Support Window
        PFD_SUPPORT_OPENGL      |         // Format Must Support OpenGL
        PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
        PFD_DOUBLEBUFFER,                 // Must Support Double Buffering
        PFD_TYPE_RGBA,                    // Request An RGBA Format
        32,                               // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
        8,                                // An Alpha Buffer
        0,                                // Shift Bit Ignored
        0,                                // No Accumulation Buffer
        0, 0, 0, 0,                       // Accumulation Bits Ignored
        24,                               // 16Bit Z-Buffer (Depth Buffer)
        8,                                // Some Stencil Buffer
        0,                                // No Auxiliary Buffer
        PFD_MAIN_PLANE,                   // Main Drawing Layer
        0,                                // Reserved
        0, 0, 0                           // Layer Masks Ignored
    };

    HDC hdc = GetDC(m_hWnd);
    int PixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (PixelFormat == 0) {
        MessageBox(NULL, "ChoosePixelFormat - failed", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    //BOOL bResult = SetPixelFormat(hdc, PixelFormat, &pfd);
    if (!SetPixelFormat(hdc, PixelFormat, &pfd)) {
        MessageBox(NULL, "SetPixelFormat - failed", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    m_hrc = wglCreateContext(hdc);
    if (!m_hrc){
        MessageBox(NULL, "wglCreateContext - failed", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ReleaseDC(m_hWnd, hdc);

    return true;
}

void drawTestTriangle() {
    glPushMatrix();

    glColor3f(0, 1, 1);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -0.99f, 0.0f); // horizontal lines at the 1.0/-1.0 boundaries don't show up
    glVertex3f(-1.0f, -0.99f, 0.0f);
    glEnd();

    glPopMatrix();
}

}