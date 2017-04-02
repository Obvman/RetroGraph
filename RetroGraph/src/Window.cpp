#include "../headers/Window.h"

#define GLUT_DISABLE_ATEXIT_HACK

#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <dwmapi.h>
#include <Windowsx.h>
#include <iostream>
#include <thread>

#include "../headers/RetroGraph.h"
#include "../headers/resource.h"
#include "../headers/utils.h"
#include "../headers/colors.h"

namespace rg {

constexpr int32_t WM_NOTIFY_RG_TRAY{ 3141 };

constexpr int32_t ID_EXIT{ 1 };
constexpr int32_t ID_SEND_TO_BACK{ 2 };
constexpr int32_t ID_RESET_POSITION{ 3 };
constexpr int32_t ID_CHANGE_DISPLAY_MONITOR{ 4 };

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    // Get the Window pointer from the handle, and call the alternate WndProc
    // if it was found
    Window* window = (Window*)GetWindowLong(hWnd, GWLP_USERDATA);
    if (window) {
        return window->WndProc2(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(RetroGraph* rg_, HINSTANCE hInstance, int32_t startupMonitor,
               bool clickthrough) : 
    m_clickthrough{ clickthrough },
    m_monitors{ },
    m_retroGraph{ rg_ },
    m_currMonitor{ startupMonitor },
    m_width{ m_monitors.getWidth(m_currMonitor) },
    m_height{ m_monitors.getHeight(m_currMonitor) },
    m_startPosX{ m_monitors.getX(m_currMonitor) },
    m_startPosY{ m_monitors.getY(m_currMonitor) },
    m_hInstance{ (createWindow(), hInstance) } {

    createTrayIcon();
}

Window::~Window() {
    std::cout << "Window releasing\n";

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_hrc);

    Shell_NotifyIcon(NIM_DELETE, &m_tray);
    PostQuitMessage(0);
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
        case WM_NOTIFY_RG_TRAY:
            handleTrayMessage(hWnd, wParam, lParam);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_EXIT:
                    SendMessage(hWnd, WM_QUIT, wParam, lParam);
                    break;
                case ID_SEND_TO_BACK: {
                    RECT wndRect;
                    GetWindowRect(hWnd, &wndRect);
                    SetWindowPos(hWnd, HWND_BOTTOM, wndRect.left, wndRect.top,
                                 m_width, m_height, 0);
                    break;
                }
                case ID_RESET_POSITION: {
                    const auto& md{ m_monitors.getMonitorData()[m_currMonitor] };
                    SetWindowPos(hWnd, HWND_TOPMOST, md.x, md.y,
                                 md.width, md.height, 0);
                    break;
                }
                default:
                    // Default case handles monitor selection list
                     changeMonitor(hWnd, LOWORD(wParam) - ID_CHANGE_DISPLAY_MONITOR);
                     break;
            }
        case WM_CONTEXTMENU: {
            int32_t contextSpawnX{ LOWORD(lParam) };
            int32_t contextSpawnY{ HIWORD(lParam) };
            // The lParam we receive is an unsigned int, but we can open
            // the context menu on a monitor with a negative x or y value,
            // so check if the value is greater than a signed short, if so
            // it should be a negative number
            if (LOWORD(lParam) > std::numeric_limits<int16_t>::max()) {
                contextSpawnX = LOWORD(lParam) - std::numeric_limits<uint16_t>::max();
            }
            if (HIWORD(lParam) > std::numeric_limits<int16_t>::max()) {
                contextSpawnY = HIWORD(lParam) - std::numeric_limits<uint16_t>::max();
            }

            createRClickMenu(reinterpret_cast<HWND>(wParam),
                             contextSpawnX, contextSpawnY);
            return 0;
        }
        case WM_NCHITTEST: {
            /*if (!m_dragging) {
                POINT p;
                p.x = LOWORD(lParam);
                p.y = HIWORD(lParam);
                ScreenToClient(m_hWndMain, &p);
                p.y = m_height - p.y;

                unsigned char pixels[4];
                glReadPixels(p.x, p.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, 
                             &pixels[0]);

                // If the pixel is the background color, we don't want to drag
                if (pixels[0] == BGCOLOR_R &&
                    pixels[1] == BGCOLOR_B &&
                    pixels[2] == BGCOLOR_G &&
                    pixels[3] == BGCOLOR_A) {

                    return HTTRANSPARENT;
                } else {
                    ;
                }
            }*/
            break;
        }
        case WM_SETCURSOR:
            break;
        case WM_LBUTTONDOWN:
            clickX = LOWORD(lParam);
            clickY = HIWORD(lParam);
            handleClick(clickX, clickY);
            return 0;
        case WM_LBUTTONUP:
            if (m_dragging) {
                m_dragging = false;
                ReleaseCapture();
            }
            return 0;
        case WM_MOUSEMOVE: {
            if (m_dragging) {
                // Use mouse and window location to drag the window under the cursor
                int mouseX = LOWORD(lParam);
                int mouseY = HIWORD(lParam);

                RECT wndRect;
                GetWindowRect(hWnd, &wndRect);

                int windowX = wndRect.left + mouseX - clickX;
                int windowY = wndRect.top + mouseY - clickY;

                SetWindowPos(hWnd, nullptr, windowX, windowY, 0, 0, SWP_NOSIZE);
            }
            return 0;
        }
        case WM_QUIT:
        case WM_CLOSE:
            m_running = false;
            break;
        case WM_DESTROY:
            break;
        default:
            break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::createRClickMenu(HWND hWnd, int32_t spawnX, int32_t spawnY) {
    auto hPopupMenu{ CreatePopupMenu() };
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SEND_TO_BACK, 
            "Send to back");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_RESET_POSITION,
            "Reset Position");

    // Create an option for each monitor for multi-monitor systems
    const auto& md{ m_monitors.getMonitorData() };
    if (m_monitors.getNumMonitors() > 1) {
        for (auto i = size_t{ 0U }; i < md.size(); ++i) {
            char optionName[] = "Move to display 0";
            optionName[16] = '0' + static_cast<char>(i);
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING,
                    ID_CHANGE_DISPLAY_MONITOR + i, optionName);
        }
    }

    // Display menu and wait for user's selection
    TrackPopupMenuEx(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN |
            TPM_RIGHTBUTTON, spawnX, spawnY, hWnd, nullptr);
}

void Window::handleClick(DWORD clickX, DWORD clickY) {
    // Origin is at top left for click coords, so convert to bottom left
    clickY = m_height - clickY;

    // Get the RGB value at the point clicked
    unsigned char pixels[4];
    glReadPixels(clickX, clickY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    // If the pixel is the background color, we don't want to drag
    if (pixels[0] == BGCOLOR_R &&
        pixels[1] == BGCOLOR_B &&
        pixels[2] == BGCOLOR_G &&
        pixels[3] == BGCOLOR_A) {
        m_dragging = false;
    } else {
        m_dragging = true;
        SetCapture(m_hWndMain);
    }
}

void Window::changeMonitor(HWND hWnd, uint32_t monIndex) {
    // Check monitor selection is in range and the monitor isn't
    // the currently selected one
    if (monIndex >= 0 &&
        monIndex < m_monitors.getNumMonitors() &&
        monIndex != static_cast<uint32_t>(m_currMonitor)) {

        m_currMonitor = monIndex;

        const auto& md{ m_monitors.getMonitorData()[m_currMonitor] };

        updateSize(md.width, md.height);
        SetWindowPos(hWnd, HWND_TOP, md.x, md.y, md.width, md.height, 0);
    }
}

void Window::handleTrayMessage(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(lParam)) {
        case WM_LBUTTONUP:
            SetForegroundWindow(hWnd);
            break;
        case WM_RBUTTONUP: {
            POINT p;
            GetCursorPos(&p);
            SetForegroundWindow(hWnd);

            // Create options for popup menu
            auto hPopupMenu{ CreatePopupMenu() };
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SEND_TO_BACK, "Send to back");

            // Create an option for each monitor for multi-monitor systems
            const auto& md{ m_monitors.getMonitorData() };
            if (m_monitors.getNumMonitors() > 1) {
                for (auto i = size_t{ 0U }; i < md.size(); ++i) {
                    char optionName[] = "Move to display 0";
                    optionName[16] = '0' + static_cast<char>(i);
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING,
                            ID_CHANGE_DISPLAY_MONITOR + i, optionName);
                }
            }

            // Display menu and wait for user's selection
            const int32_t selection{ TrackPopupMenuEx(hPopupMenu,
                    TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                    p.x, p.y, hWnd, nullptr) };

            switch (selection) {
                case ID_EXIT:
                    SendMessage(hWnd, WM_QUIT, wParam, lParam);
                    break;
                case ID_SEND_TO_BACK:
                    RECT wndRect;
                    GetWindowRect(hWnd, &wndRect);
                    SetWindowPos(hWnd, HWND_BOTTOM, wndRect.left, wndRect.top,
                                 m_width, m_height, 0);
                    break;
                default:
                    // Default case handles monitor selection list
                     changeMonitor(hWnd, selection - ID_CHANGE_DISPLAY_MONITOR);
                     break;
            }

            break;
        }
        default:
            break;
    }
}

void Window::updateSize(int32_t width, int32_t height) {
    m_width = width;
    m_height = height;
    m_retroGraph->updateWindowSize(width, height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

HDC Window::startDraw() const {
    HDC hdc = GetDC(m_hWndMain);
    wglMakeCurrent(hdc, m_hrc);
    glViewport(0, 0, m_width, m_height);
    return hdc;
}

void Window::endDraw(HDC hdc) const {
    SwapBuffers(hdc);
    ReleaseDC(m_hWndMain, hdc);
}

void Window::createWindow() {
    // Set and register the window class
    memset(&m_wc, 0, sizeof(m_wc));
    m_wc.cbSize = sizeof(WNDCLASSEX);
    m_wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    m_wc.style = CS_HREDRAW | CS_VREDRAW;
    m_wc.lpfnWndProc = WndProc;
    m_wc.cbClsExtra = 0;
    m_wc.cbWndExtra = 0;
    m_wc.hInstance = m_hInstance;
    m_wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    m_wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
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

void Window::createTrayIcon() {
    m_tray.cbSize = sizeof(m_tray);
    m_tray.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    m_tray.hWnd = m_hWndMain;
    m_tray.uVersion = NOTIFYICON_VERSION_4;
#if _DEBUG
    strcpy_s(m_tray.szTip, 128, "RetroGraph (Debug)");
#else
    strcpy_s(m_tray.szTip, 128, "RetroGraph");
#endif
    m_tray.uCallbackMessage = WM_NOTIFY_RG_TRAY;
    m_tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    m_tray.uID = 101;

    if (!Shell_NotifyIcon(NIM_ADD, &m_tray)) {
        fatalMessageBox("Failed to display notification icon");
    } 
}

void Window::initOpenGL() {
    glewInit();

    // Make dummy command line arguments for glutInit
    char* gargv[1] = {""};
    int gargc{ 1 };
    glutInit(&gargc, gargv);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    glEnable(GL_BLEND); // Alpha blending needed for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

}

bool Window::createHGLRC() {
    /* Much of this function and its children are based on the work by
       Colt McAnlis. Code and explanations:
       http://nehe.gamedev.net/tutorial/fullscreen_antialiasing/16008/ */

#if _DEBUG
    const char* windowName{ "RetroGraph (Debug)" };
#else
    const char* windowName{ "RetroGraph" };
#endif

    if (m_arbMultisampleSupported) {
        const DWORD exStyles = (m_clickthrough) ? 
            WS_EX_TOOLWINDOW | WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_LAYERED :
            WS_EX_TRANSPARENT | WS_EX_COMPOSITED;

        // Create main window with clicking through to windows underneath, 
        // no taskbar display, and transparency
        m_hWndMain = CreateWindowEx( exStyles, "RetroGraph", windowName,
                WS_VISIBLE | WS_POPUP,
                m_startPosX, m_startPosY, m_width, m_height,
                nullptr, nullptr, m_hInstance, nullptr);
    } else {
        // Create test window that doesn't appear in taskbar to query
        // anti-aliasing capabilities
        m_hWndMain = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
                "RetroGraph", "Dummy",
                WS_VISIBLE | WS_POPUP,
                m_startPosX, m_startPosY, m_width, m_height,
                nullptr, nullptr, m_hInstance, nullptr);
    }

    // Store a pointer to this Window object so we can reference members in WndProc
    SetWindowLong(m_hWndMain, GWLP_USERDATA, (LONG)this);

    #if (!_DEBUG)
    // Display window at the desktop layer on startup
    SetWindowPos(m_hWndMain, HWND_BOTTOM, 
            m_startPosX, m_startPosY, m_width, m_height, 0);
    #endif

    if(!m_hWndMain) {
        fatalMessageBox("CreateWindowEx - failed");
    }

    // Make window transparent via dwm
    const auto hRgn{ CreateRectRgn(0, 0, -1, -1) };
    DWM_BLURBEHIND bb = {0};
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
        PFD_DRAW_TO_WINDOW      |
        PFD_SUPPORT_OPENGL      |
        PFD_SUPPORT_COMPOSITION |         // For transparency
        PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,                               // Color Depth
        0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
        8,                                // Alpha Buffer
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
    }

    // Create an opengl context for the window
    m_hrc = wglCreateContext(m_hdc);
    if (!m_hrc){
        ReleaseDC(m_hWndMain, m_hdc);
        m_hdc = 0;
        DestroyWindow(m_hWndMain);
        m_hWndMain = 0;
        fatalMessageBox("wglCreateContext - failed");
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
    }

    // Once we've created the first window and found multisampling to be
    // supported, we can destroy it and create a second window with a different
    // pixel format that has multisampling
    if (!m_arbMultisampleSupported) {
        if (initMultisample()) {
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

bool Window::initMultisample() {
    // See If The String Exists In WGL
    if (!wglIisExtensionSupported("WGL_ARB_multisample")) {
        m_arbMultisampleSupported = false;
        return false;
    }

    // Get Our Pixel Format
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
        reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(
                wglGetProcAddress("wglChoosePixelFormatARB")
        );
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

}
