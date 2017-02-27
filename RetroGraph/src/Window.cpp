#include "../headers/Window.h"

#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <dwmapi.h>
#include <iostream>
#include <thread>

#include "../headers/resource.h"
#include "../headers/utils.h"
#include "../headers/colors.h"


namespace rg {

void drawBorder();

Window::Window(HINSTANCE hInstance, const char* windowName,
               uint16_t width, uint16_t height, uint16_t startX, uint16_t startY) :
    m_hInstance{ hInstance },
    m_width{ width },
    m_height{ height },
    m_startPosX{ startX },
    m_startPosY{ startY },
    m_cpuMeasure{ m_width, m_height/3 },
    m_gpuMeasure{ },
    m_ramMeasure{ },
    m_processMeasure{ },
    m_driveMeasure{},
    m_systemInfo{ },
    m_renderer{ m_cpuMeasure, m_gpuMeasure, m_ramMeasure, m_processMeasure,
                m_driveMeasure, m_systemInfo, m_width, m_height },
    m_arbMultisampleSupported{ false },
    m_arbMultisampleFormat{ 0 },
    m_aaSamples{ 8 },
    m_shaders{} {

    WNDCLASSEX m_wc;
    memset(&m_wc, 0, sizeof(m_wc));
    m_wc.cbSize = sizeof(WNDCLASSEX);
    m_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    m_wc.style = CS_HREDRAW | CS_VREDRAW;
    m_wc.lpfnWndProc = Window::WndProc;
    m_wc.cbClsExtra  = 0;
    m_wc.cbWndExtra  = 0;
    m_wc.hInstance = m_hInstance;
    m_wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    m_wc.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
    m_wc.lpszClassName = windowName;

    if(!RegisterClassEx(&m_wc)) {
        fatalMessageBox("RegisterClassEx - failed");
    }

    if (!createHGLRC()) {
        fatalMessageBox("Failed to create OpenGL Window");
    }

    initOpenGL();
    updateSize(m_width, m_height);

    ReleaseDC(m_hWndMain, m_hdc);
}


Window::~Window() {
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static PAINTSTRUCT ps;
    switch (msg) {
        case WM_PAINT:
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            return 0;
        case WM_SIZE:
            glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
            PostMessage(hWnd, WM_PAINT, 0, 0);
            return 0;
        case WM_WINDOWPOSCHANGING:
            break;
        case WM_WINDOWPOSCHANGED:
            break;
        case WM_CREATE:
            break;
        case WM_SETCURSOR:
            break;
        case WM_NCHITTEST: {
            // TODO: limit how often this message is handled to lower CPU usage while dragging
            LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
            if (hit == HTCLIENT) hit = HTCAPTION;
            return hit;
        }
        case WM_MOUSEMOVE:
            break;
        case WM_GETTEXT:
            break;
        case WM_QUIT:
            PostQuitMessage(0);
            exit(0);
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            exit(0);
            break;
        case WM_DESTROY:
            return 0;
        default:
            //std::cout << "Handling message code: " << msg << '\n';
            break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::show() {
    ShowWindow(m_hWndMain, SW_SHOW);
}

void Window::init() {
    m_cpuMeasure.update();
    m_gpuMeasure.update();
    m_ramMeasure.update();

    m_processMeasure.init();
    m_driveMeasure.init();

    draw();
}

void Window::update(uint32_t ticks) {
    // half-second updates
    if ((ticks % (ticksPerSecond/2)) == 0) {
        m_cpuMeasure.update();
        m_gpuMeasure.update();
        m_ramMeasure.update();
    }

    // Full second updates
    if ((ticks % (ticksPerSecond * 2)) == 0) {
        m_driveMeasure.update(ticks);
    }

    m_processMeasure.update(ticks);

}

void Window::draw() const {
    HDC hdc = GetDC(m_hWndMain);
    wglMakeCurrent(hdc, m_hrc);

    m_renderer.draw(m_shaders);

    //drawBorder();

    SwapBuffers(hdc);
    ReleaseDC(m_hWndMain, hdc);
}

void Window::updateSize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Window::initOpenGL() {
    glewInit();

    // Make dummy command line arguments for glutInit
    char* gargv[1] = {""};
    int gargc{ 1 };
    glutInit(&gargc, gargv);

    glEnable(GL_ALPHA_TEST);
    //glEnable(GL_DEPTH_TEST); // 3D depth testing
    glEnable(GL_MULTISAMPLE_ARB);
    glEnable(GL_BLEND); // Alpha blending needed for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    m_renderer.init(m_hWndMain);

    m_shaders.loadShaders();

    m_systemInfo.getGPUDescription();
}

void Window::releaseOpenGL() {
    std::cout << "Releasing OpenGL\n";
    m_renderer.release();
    //glDeleteLists(1000, 256);

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_hrc);
}

bool Window::createHGLRC() {
    /* Much of this function and its children are based on the work by
       Colt McAnlis. Code and explanations:
       http://nehe.gamedev.net/tutorial/fullscreen_antialiasing/16008/ */

    m_hWndMain = CreateWindowEx(WS_EX_APPWINDOW, "RetroGraph", "RetroGraph",
                                WS_VISIBLE | WS_POPUP, m_startPosX, m_startPosY, m_width, m_height,
                                NULL, NULL, m_hInstance, NULL);

    if (m_arbMultisampleSupported && false) {
        // Show the second window in the toolbar
        auto currStyle = GetWindowLong(m_hWndMain, GWL_STYLE);
        currStyle &= ~WS_EX_TOOLWINDOW;
        //currStyle |= WS_EX_APPWINDOW;
        SetWindowLong(m_hWndMain, GWL_STYLE, WS_EX_APPWINDOW);

    }


    #if (!_DEBUG)
    // Display window at the desktop layer on startup
    SetWindowPos(m_hWndMain, HWND_BOTTOM, m_startPosX, m_startPosY, m_width, m_height, 0);
    #endif

    if(!m_hWndMain) {
        fatalMessageBox("CreateWindowEx - failed");
    }

    // Make window transparent via dwm
    DWM_BLURBEHIND bb = {0};
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(m_hWndMain, &bb);

    m_hdc = GetDC(m_hWndMain);
    if (m_hdc == 0) {
        DestroyWindow(m_hWndMain);
        m_hWndMain = 0;
        return false;
    }

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

    // On the first run, create a regular window with the previous pfd
    // If multisampling is supported on the second run, use the alternate
    // pfd to create an anti-aliased window
    int32_t PixelFormat;
    if (!m_arbMultisampleSupported) {
        PixelFormat = ChoosePixelFormat(m_hdc, &pfd);
        if (PixelFormat == 0) {
            ReleaseDC(m_hWndMain, m_hdc);
            DestroyWindow(m_hWndMain);
            fatalMessageBox("ChoosePixelFormat - failed");
            return false;
        }
    } else {
        PixelFormat = m_arbMultisampleFormat;
    }

    // Set the pixel format for the window
    if (!SetPixelFormat(m_hdc, PixelFormat, &pfd)) {
        ReleaseDC(m_hWndMain, m_hdc);
        m_hdc = 0;
        DestroyWindow(m_hWndMain);
        m_hWndMain = 0;
        fatalMessageBox("SetPixelFormat - failed");
        return false;
    }

    // Create an opengl context for the window
    m_hrc = wglCreateContext(m_hdc);
    if (!m_hrc){
        ReleaseDC(m_hWndMain, m_hdc);
        m_hdc = 0;
        DestroyWindow(m_hWndMain);
        m_hWndMain = 0;
        fatalMessageBox("wglCreateContext - failed");
        return false;
    }

    // Make the context the current one for the window
    if (!wglMakeCurrent(m_hdc, m_hrc)) {
        wglDeleteContext(m_hrc);
        m_hrc = 0;
        ReleaseDC(m_hWndMain, m_hdc);
        m_hdc = 0;
        DestroyWindow(m_hWndMain);
        m_hWndMain = 0;

        fatalMessageBox("Failed to make current context with wglMakeCurrent");
        return false;
    }

    // Once we've created the first window and found multisampling to be
    // supported, we can destroy it and create a second window with a different
    // pixel format that has multisampling
    if (!m_arbMultisampleSupported) {
        if (initMultisample(pfd)) {
            destroy();
            return createHGLRC();
        }
    }

    return true;
}
bool Window::wglIisExtensionSupported(const char *extension) {
    const size_t extlen{ strlen(extension) };
    const char *supported{ nullptr };

    // Try To Use wglGetExtensionStringARB On Current DC, If Possible
    PROC wglGetExtString{ wglGetProcAddress("wglGetExtensionsStringARB") };

    if (wglGetExtString) {
        supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());
    }

    // If That Failed, Try Standard Opengl Extensions String
    if (!supported) {
        supported = (char*)glGetString(GL_EXTENSIONS);
    }

    // If That Failed Too, Must Be No Extensions Supported
    if (!supported) {
        return false;
    }

    // Begin Examination At Start Of String, Increment By 1 On False Match
    for (const char* p = supported; ; p++) {
        // Advance p Up To The Next Possible Match
        p = strstr(p, extension);
        if (!p) {
            return false;
        }

        if ((p == supported || p[-1] == ' ') &&
            (p[extlen] == '\0' || p[extlen] == ' ')) {

            return true;
        }
    }
}
bool Window::initMultisample(PIXELFORMATDESCRIPTOR& pfd) {
    // See If The String Exists In WGL
    if (!wglIisExtensionSupported("WGL_ARB_multisample")) {
        m_arbMultisampleSupported = false;
        return false;
    }

    // Get Our Pixel Format
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
        (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    if (!wglChoosePixelFormatARB) {
        m_arbMultisampleSupported = false;
        return false;
    }

    // Get Our Current Device Context
    HDC hDC = GetDC(m_hWndMain);
    int32_t pixelFormat;
    int32_t valid;
    uint32_t numFormats;
    float fAttributes[] = { 0, 0 };

    // These Attributes Are The Bits We Want To Test For In Our Sample
    // Everything Is Pretty Standard, The Only One We Want To
    // Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
    // These Two Are Going To Do The Main Testing For Whether Or Not
    // We Support Multisampling On This Hardware.
    int32_t iAttributes[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 24,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 16,
        WGL_STENCIL_BITS_ARB, 0,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, m_aaSamples,
        0, 0
    };

    // First We Check To See If We Can Get A Pixel Format For 8 Samples
    valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1,
                                    &pixelFormat, &numFormats);

    // If We Returned True, And Our Format Count Is Greater Than 1
    if (valid && numFormats >= 1) {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return m_arbMultisampleSupported;
    }

    // Our Pixel Format With 8 Samples Failed, Test For 4 Samples
    iAttributes[19] = 4;
    valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1,
                                    &pixelFormat,&numFormats);
    if (valid && numFormats >= 1) {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return m_arbMultisampleSupported;
    }

    // Our Pixel Format With 2 Samples Failed, Test For 2 Samples
    iAttributes[19] = 2;
    valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1,
                                    &pixelFormat,&numFormats);
    if (valid && numFormats >= 1) {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return m_arbMultisampleSupported;
    }

    return m_arbMultisampleSupported;
}

void Window::destroy() {
    // Release and zero the device context, openGL context and window handle
    if (m_hWndMain != 0) {
        if (m_hdc != 0) {
            wglMakeCurrent(m_hdc, 0);
            if (m_hrc != 0) {
                wglDeleteContext(m_hrc);
                m_hrc = 0;
            }

            ReleaseDC(m_hWndMain, m_hdc);
            m_hdc = 0;
        }

        DestroyWindow(m_hWndMain);
        m_hWndMain = 0;
    }
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
