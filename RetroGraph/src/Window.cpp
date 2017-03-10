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

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window(HINSTANCE hInstance) :
    m_userSettings{ },
    m_hInstance{ hInstance },
    m_dragging{ false },
    m_width{ 0 },
    m_height{ 0 },
    m_startPosX{ 0 },
    m_startPosY{ 0 },
    m_arbMultisampleSupported{ false },
    m_arbMultisampleFormat{ 0 },
    m_aaSamples{ 8 },
    m_cpuMeasure{ },
    m_gpuMeasure{ },
    m_ramMeasure{ },
    m_processMeasure{ },
    m_driveMeasure{ },
    m_netMeasure{},
    m_systemInfo{ },
    m_renderer{ m_cpuMeasure, m_gpuMeasure, m_ramMeasure, m_netMeasure,
                m_processMeasure, m_driveMeasure, m_systemInfo }
{
}


Window::~Window() {
}

constexpr int32_t ID_EXIT{ 1 };
constexpr int32_t ID_SEND_TO_BACK{ 2 };
constexpr int32_t ID_RESET_POS{ 3 };

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    // Get the Window pointer from the handle, and call the alternate WndProc
    // if it was found
    Window* window = (Window*)GetWindowLong(hWnd, GWLP_USERDATA);
    if (window) {
        return window->WndProc2(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::WndProc2(HWND hWnd, UINT msg,
                                  WPARAM wParam, LPARAM lParam) {
    static int clickX;
    static int clickY;
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
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_EXIT:
                    SendMessage(hWnd, WM_QUIT, wParam, lParam);
                    break;
                case ID_SEND_TO_BACK:
                    SetWindowPos(hWnd, HWND_BOTTOM, m_startPosX, m_startPosY,
                                 m_width, m_height, 0);
                    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                    break;
                case ID_RESET_POS:
                    SetWindowPos(hWnd, HWND_BOTTOM, m_startPosX, m_startPosY,
                                 m_width, m_height, 0);
                    break;
            }
        case WM_CONTEXTMENU:
            createRClickMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_SETCURSOR:
            break;
        case WM_LBUTTONDOWN:
            m_dragging = true;
            SetCapture(hWnd);
            clickX = LOWORD(lParam);
            clickY = HIWORD(lParam);
            break;
        case WM_LBUTTONUP:
            m_dragging = false;
            ReleaseCapture();
            break;
        case WM_MOUSEMOVE: {
            if (m_dragging) {
                int mouseX = LOWORD(lParam);
                int mouseY = HIWORD(lParam);

                RECT wndRect;
                GetWindowRect(hWnd, &wndRect);

                int windowX = wndRect.left + mouseX - clickX;
                int windowY = wndRect.top + mouseY - clickY;

                //std::cout << "Position: " << pos.x << ", " << pos.y << '\n';
                //ClientToScreen(hWnd, &pos);
                SetWindowPos(hWnd, nullptr, windowX, windowY, 0, 0, SWP_NOSIZE);

            }
            break;
        }
        case WM_GETTEXT:
            break;
        case WM_QUIT:
            PostQuitMessage(0);
            releaseOpenGL();
            exit(0);
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            releaseOpenGL();
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

void Window::createRClickMenu(HWND hWnd, DWORD cursorX, DWORD cursorY) {
    auto hPopupMenu{ CreatePopupMenu() };
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SEND_TO_BACK, "Send to back");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_RESET_POS, "Reset position");
    SetForegroundWindow(hWnd);

    TrackPopupMenuEx(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                   cursorX, cursorY, hWnd, NULL);
}

void Window::init() {
    // Initialise userSettings with config file data
    m_userSettings.init();
    m_width = m_userSettings.getWindowWidth();
    m_height = m_userSettings.getWindowHeight();
    m_startPosX = m_userSettings.getWindowX();
    m_startPosY = m_userSettings.getWindowY();

    createWindow();

    m_cpuMeasure.init();
    m_gpuMeasure.init();
    m_ramMeasure.init();
    m_processMeasure.init();
    m_driveMeasure.init();
    m_systemInfo.init();
    m_netMeasure.init(m_userSettings.getNetAdapterName());

    m_renderer.init(m_hWndMain, m_width, m_height);

    draw(0); // TODO change this value to guarantee drawing in the very first frame
}

void Window::update(uint32_t ticks) {
    // half-second updates
    if ((ticks % (ticksPerSecond/2)) == 0) {
        m_cpuMeasure.update();
        m_gpuMeasure.update();
        m_ramMeasure.update();
        m_netMeasure.update();
    }

    // Full second updates
    if ((ticks % (ticksPerSecond * 2)) == 0) {
        m_driveMeasure.update(ticks);
    }

    m_processMeasure.update(ticks);

}

void Window::draw(uint32_t ticks) const {
    HDC hdc = GetDC(m_hWndMain);
    wglMakeCurrent(hdc, m_hrc);

    m_renderer.draw(ticks);

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

void Window::createWindow() {
    // Set and register the window class
    memset(&m_wc, 0, sizeof(m_wc));
    m_wc.cbSize = sizeof(WNDCLASSEX);
    m_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    m_wc.style = CS_HREDRAW | CS_VREDRAW;
    m_wc.lpfnWndProc = WndProc;
    m_wc.cbClsExtra = 0;
    m_wc.cbWndExtra = 0;
    m_wc.hInstance = m_hInstance;
    m_wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    m_wc.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(0x00000000));
    m_wc.lpszClassName = "RetroGraph";

    if (!RegisterClassEx(&m_wc)) {
        fatalMessageBox("RegisterClassEx - failed");
    }

    /* Build the window itself*/
    if (!createHGLRC()) {
        fatalMessageBox("Failed to create OpenGL Window");
    }

    initOpenGL();

    updateSize(m_width, m_height);

    ReleaseDC(m_hWndMain, m_hdc);
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

    if (m_arbMultisampleSupported) {
        m_hWndMain = CreateWindowEx(WS_EX_APPWINDOW, "RetroGraph", "RetroGraph",
                                    WS_VISIBLE | WS_POPUP, m_startPosX, m_startPosY, m_width, m_height,
                                    NULL, NULL, m_hInstance, NULL);
    } else {
        // Create test window that doesn't appear in taskbar to query
        // anti-aliasing capabilities
        m_hWndMain = CreateWindowEx(WS_EX_TOOLWINDOW, "RetroGraph", "Dummy",
                                    WS_VISIBLE | WS_POPUP, m_startPosX, m_startPosY, m_width, m_height,
                                    NULL, NULL, m_hInstance, NULL);
    }

    // Store a pointer to this Window object that we can later recover in WndProc
    SetWindowLong(m_hWndMain, GWLP_USERDATA, (LONG)this);

    #if (!_DEBUG)
    // Display window at the desktop layer on startup
    SetWindowPos(m_hWndMain, HWND_BOTTOM, m_startPosX, m_startPosY, m_width, m_height, 0);
    #endif

    if(!m_hWndMain) {
        fatalMessageBox("CreateWindowEx - failed");
    }

    // Make window transparent via dwm
    DWM_BLURBEHIND bb = {0};
    const auto hRgn = CreateRectRgn(0, 0, -1, -1);
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
    const auto extlen{ strlen(extension) };
    const char *supported{ nullptr };

    // Try To Use wglGetExtensionStringARB On Current DC, If Possible
    const auto wglGetExtString{ wglGetProcAddress("wglGetExtensionsStringARB") };

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
        reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
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
