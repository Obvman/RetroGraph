#include "Window.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <dwmapi.h>
#include <iostream>

#include "utils.h"
#include "colors.h"

// Must draw the lines a slight distance from the window edge otherwise they
// won't show up
constexpr float bDelta{ 0.0001f };

namespace rg {

void drawBorder();

Window::Window(HINSTANCE hInstance, const char* windowName) :
    m_width{ 480U },
    m_height{ 480U },
    m_cpuMeasure{ },
    m_gpuMeasure{ },
    m_ramMeasure{ },
    m_processMeasure{ } {

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
        fatalMessageBox("RegisterClassEx - failed");
    }

    m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, "RetroGraph", "RetroGraph",
                               WS_VISIBLE | WS_POPUP, 0, 0, m_width, m_height,
                               NULL, NULL, hInstance, NULL);

    if(!m_hWnd) {
        fatalMessageBox("CreateWindowEx - failed");
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
    updateSize(m_width, m_height);

    ReleaseDC(m_hWnd, m_hdc);
}


Window::~Window() {
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Do some silly casting magic so this function can access Window's members
    // using the pThis pointer
    Window* pThis{ nullptr };
    if (msg == WM_CREATE) {
        pThis = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(pThis))) {
            if (GetLastError() != 0) {
                showMessageBox("Failed to set window pointer");
                return false;
            }
        }
    } else {
        pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWL_USERDATA));
    }

    static PAINTSTRUCT ps;
    switch (msg) {
        case WM_PAINT:
            /*std::cout << "Painting\n";
            pThis->draw();*/
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
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
            break;
        case WM_NCHITTEST: {
            // TODO: limit how often this message is handled to lower CPU usage while dragging
            LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
            if (hit == HTCLIENT) hit = HTCAPTION;
            return hit;
            break;
        }
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

void Window::show() {
    ShowWindow(m_hWnd, SW_SHOW);
}

void Window::init() {
    m_cpuMeasure.update();
    m_gpuMeasure.update();
    m_ramMeasure.update();
    m_processMeasure.update();

    draw();
}

void Window::update(uint16_t ticks) {
    if (ticks % 5 == 0) {
        m_cpuMeasure.update();
        m_gpuMeasure.update();
        m_ramMeasure.update();
    }

    // Only update processes each second
    if (ticks % 10 == 0) {
        m_processMeasure.update();
    }
}

void Window::draw() const {
    auto floats = m_cpuMeasure.m_usageData;

    HDC hdc = GetDC(m_hWnd);
    wglMakeCurrent(hdc, m_hrc);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();

    glBegin(GL_LINES); {
        // Draw each node in the graph
        for (auto i{ 0U }; i < floats.size() - 1; ++i) {
            glColor3f(BLUE1_R, BLUE1_G, BLUE1_B);

            float x1 = (i / static_cast<float>(floats.size() - 1)) * 2.0f - 1.0f;
            float x2 = ((i + 1) / static_cast<float>(floats.size() - 1)) * 2.0f - 1.0f;

            float y1 = floats[i] * 2.0f - 1.0f;
            float y2 = floats[i + 1] * 2.0f - 1.0f;

            // If the vertex is at the border, add/subtract the border delta
            if (i == 0) {
                x1 += bDelta;
            } else if (i == floats.size() - 2) {
                x2 -= bDelta;
            }

            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x2, y2, 0.0f);
        }
    } glEnd();
    glPopMatrix();

    drawBorder();

    SwapBuffers(hdc);
    ReleaseDC(m_hWnd, hdc);
}

void Window::updateSize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Window::initOpenGL() const {
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_MULTISAMPLE);

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
        fatalMessageBox("ChoosePixelFormat - failed");
        return false;
    }

    if (!SetPixelFormat(hdc, PixelFormat, &pfd)) {
        fatalMessageBox("SetPixelFormat - failed");
        return false;
    }

    m_hrc = wglCreateContext(hdc);
    if (!m_hrc){
        fatalMessageBox("wglCreateContext - failed");
        return false;
    }

    ReleaseDC(m_hWnd, hdc);

    return true;
}


void drawBorder() {

    glColor3f(PINK1_R, PINK1_G, PINK1_B);
    glBegin(GL_LINES);
    glVertex2f(-1.0f + bDelta, -1.0f + bDelta);
    glVertex2f(-1.0f + bDelta,  1.0f - bDelta);

    glVertex2f(-1.0f + bDelta, 1.0f - bDelta);
    glVertex2f(1.0f - bDelta, 1.0f - bDelta);

    glVertex2f(1.0f - bDelta, 1.0f - bDelta);
    glVertex2f(1.0f - bDelta, -1.0f + bDelta);

    glVertex2f(1.0f - bDelta, -1.0f + bDelta);
    glVertex2f(-1.0f + bDelta, -1.0f + bDelta);
    glEnd();
}
}
