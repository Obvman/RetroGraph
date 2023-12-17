module;

#include "Resources/resource.h"
#include "RetroGraphDLL.h"

module Window;

import Colors;
import UserSettings;

import "RGAssert.h";
import "GLHeaderUnit.h";
import "GlutHeaderUnit.h";
import "WindowsHeaderUnit.h";

namespace rg {

constexpr auto WM_NOTIFY_RG_TRAY = int{ 3141 };

constexpr auto ID_EXIT                      = int{ 1 };
constexpr auto ID_SEND_TO_BACK              = int{ 2 };
constexpr auto ID_RESET_POSITION            = int{ 3 };
constexpr auto ID_SET_WIDGET_BG             = int{ 4 };
constexpr auto ID_TEST                      = int{ 5 };
constexpr auto ID_TOGGLE_TIME_WIDGET        = int{ 6 };
constexpr auto ID_TOGGLE_HDD_WIDGET         = int{ 7 };
constexpr auto ID_TOGGLE_CPUSTATS_WIDGET    = int{ 8 };
constexpr auto ID_TOGGLE_PROCESS_CPU_WIDGET = int{ 9 };
constexpr auto ID_TOGGLE_PROCESS_RAM_WIDGET = int{ 10 };
constexpr auto ID_TOGGLE_SYSTEMSTATS_WIDGET = int{ 11 };
constexpr auto ID_TOGGLE_MAIN_WIDGET        = int{ 12 };
constexpr auto ID_TOGGLE_MUSIC_WIDGET       = int{ 13 };
constexpr auto ID_TOGGLE_FPS_WIDGET         = int{ 14 };
constexpr auto ID_TOGGLE_CPU_GRAPH_WIDGET   = int{ 15 };
constexpr auto ID_TOGGLE_GPU_GRAPH_WIDGET   = int{ 16 };
constexpr auto ID_TOGGLE_RAM_GRAPH_WIDGET   = int{ 17 };
constexpr auto ID_TOGGLE_NET_GRAPH_WIDGET   = int{ 18 };
constexpr auto ID_CHANGE_DISPLAY_MONITOR    = int{ 19 }; // Should always be last ID in the list

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Get the Window pointer from the handle, and call the alternate WndProc if it was found
    auto* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (window)
        return window->WndProc2(hWnd, msg, wParam, lParam);
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::runTest() {
    m_retroGraph->toggleWidget(WidgetType::Main);
}

Window::Window(IRetroGraph* rg_, std::shared_ptr<const DisplayMeasure> displayMeasure, HINSTANCE hInstance, int startupMonitor)
    : m_displayMeasure{ displayMeasure }
    , m_retroGraph{ rg_ }
    , m_currMonitor{ startupMonitor }
    , m_width{ m_displayMeasure->getMonitors()->getWidth(m_currMonitor)}
    , m_height{ m_displayMeasure->getMonitors()->getHeight(m_currMonitor) }
    , m_startPosX{ m_displayMeasure->getMonitors()->getX(m_currMonitor) }
    , m_startPosY{ m_displayMeasure->getMonitors()->getY(m_currMonitor) }
    , m_hInstance{ hInstance }
    , m_configRefreshedHandle{
        UserSettings::inst().configRefreshed.attach(
            [&]() {
                const auto newMonitor{ UserSettings::inst().getVal<int>("Window.Monitor") };
                if (m_currMonitor != newMonitor)
                    changeMonitor(m_hWndMain, newMonitor);
            })
    } {

    createWindow();
    createTrayIcon();
}

Window::~Window() {
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_hrc);

    Shell_NotifyIcon(NIM_DELETE, &m_tray);
    PostQuitMessage(0);
}

LRESULT CALLBACK Window::WndProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_EXIT:
                    SendMessage(hWnd, WM_QUIT, wParam, lParam);
                    break;
                case ID_SEND_TO_BACK:
                    sendToBack();
                    break;
                case ID_RESET_POSITION: {
                    const auto& md{ m_displayMeasure->getMonitors()->getMonitorData()[m_currMonitor] };
                    SetWindowPos(hWnd, HWND_TOPMOST, md.x, md.y, md.width, md.height, 0);
                    break;
                }
                case ID_SET_WIDGET_BG:
                    UserSettings::inst().toggleWidgetBackgroundVisible();
                    break;
                default:
                    // Default case handles monitor selection list
                    changeMonitor(hWnd, LOWORD(wParam) - ID_CHANGE_DISPLAY_MONITOR);
                    break;
            }
            break;
        }
        case WM_CONTEXTMENU: {
            int contextSpawnX{ LOWORD(lParam) };
            int contextSpawnY{ HIWORD(lParam) };
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

            createRClickMenu(reinterpret_cast<HWND>(wParam));
            return 0;
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
        case WM_MBUTTONUP:
            sendToBack();
            break;
        case WM_KEYDOWN:
            if constexpr (debugMode) {
                // ctrl-t runs the test command
                if ('T' == wParam && (::GetKeyState(VK_CONTROL) >> 15)) {
                    runTest();
                }
            }
            if ('R' == wParam && (::GetKeyState(VK_CONTROL) >> 15)) {
                m_retroGraph->reloadResources();
            }
            break;

        case WM_QUIT:
        case WM_CLOSE:
            m_running = false;
            m_retroGraph->shutdown();
            break;
        case WM_DESTROY:
            break;
        default:
            break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Window::createRClickMenu(HWND hWnd) {
    POINT p;
    GetCursorPos(&p);
    SetForegroundWindow(hWnd);

    // Create options for popup menu
    auto hPopupMenu{ CreatePopupMenu() };
    auto widgetSubmenu{ CreatePopupMenu() };

    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SEND_TO_BACK, "Send to back");
    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_SET_WIDGET_BG, "Toggle Widget Background");
    if constexpr (debugMode) {
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_TEST, "Test");
    }

    // Create an option for each monitor for multi-monitor systems
    const auto& md{ m_displayMeasure->getMonitors()->getMonitorData() };
    if (m_displayMeasure->getMonitors()->getNumMonitors() > 1) {
        for (auto i = size_t{ 0U }; i < md.size(); ++i) {
            char optionName[] = "Move to display 0";
            optionName[16] = '0' + static_cast<char>(i);
            InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING,
                       ID_CHANGE_DISPLAY_MONITOR + i, optionName);
        }
    }

    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, 
               reinterpret_cast<UINT_PTR>(widgetSubmenu), "Toggle Widgets");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_TIME_WIDGET, "Time Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_HDD_WIDGET, "HDD Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_CPUSTATS_WIDGET, "CPU Stats Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_PROCESS_CPU_WIDGET, "CPU Process Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_PROCESS_RAM_WIDGET, "Ram Process Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_CPU_GRAPH_WIDGET, "CPU Graph Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_GPU_GRAPH_WIDGET, "GPU Graph Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_RAM_GRAPH_WIDGET, "RAM Graph Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_NET_GRAPH_WIDGET, "Net Graph Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_SYSTEMSTATS_WIDGET, "System Stats Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_MUSIC_WIDGET, "Music Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_FPS_WIDGET, "FPS Widget");
    InsertMenu(widgetSubmenu, 0, MF_BYPOSITION | MF_STRING, ID_TOGGLE_MAIN_WIDGET, "Main Widget");

    // Display menu and wait for user's selection
    const int selection{ TrackPopupMenuEx(hPopupMenu, TPM_TOPALIGN
                                              | TPM_LEFTALIGN |
                                              TPM_RIGHTBUTTON |
                                              TPM_RETURNCMD, p.x, p.y,
                                              hWnd, nullptr) };

    switch (selection) {
        case ID_EXIT:
            // SendMessage(hWnd, WM_QUIT, wParam, lParam);Widgets
            SendMessage(hWnd, WM_QUIT, 0, 0);
            break;
        case ID_SEND_TO_BACK:
            sendToBack();
            break;
        case ID_SET_WIDGET_BG:
            UserSettings::inst().toggleWidgetBackgroundVisible();
            break;
        case ID_TEST:
            runTest();
            break;
        case ID_TOGGLE_MUSIC_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::Music);
            break;
        case ID_TOGGLE_TIME_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::Time);
            break;
        case ID_TOGGLE_HDD_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::HDD);
            break;
        case ID_TOGGLE_CPUSTATS_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::CPUStats);
            break;
        case ID_TOGGLE_PROCESS_CPU_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::ProcessCPU);
            break;
        case ID_TOGGLE_PROCESS_RAM_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::ProcessRAM);
            break;
        case ID_TOGGLE_CPU_GRAPH_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::CPUGraph);
            break;
        case ID_TOGGLE_GPU_GRAPH_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::GPUGraph);
            break;
        case ID_TOGGLE_RAM_GRAPH_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::RAMGraph);
            break;
        case ID_TOGGLE_NET_GRAPH_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::NetGraph);
            break;
        case ID_TOGGLE_SYSTEMSTATS_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::SystemStats);
            break;
        case ID_TOGGLE_MAIN_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::Main);
            break;
        case ID_TOGGLE_FPS_WIDGET:
            m_retroGraph->toggleWidget(WidgetType::FPS);
            break;
        default:
            // Default case handles monitor selection list
            changeMonitor(hWnd, selection - ID_CHANGE_DISPLAY_MONITOR);
            break;
    }
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

void Window::changeMonitor(HWND hWnd, int monIndex) {
    // Check monitor selection is in range and the monitor isn't
    // the currently selected one
    if (monIndex >= 0 &&
        monIndex < m_displayMeasure->getMonitors()->getNumMonitors() &&
        monIndex != m_currMonitor) {

        m_currMonitor = monIndex;

        const auto& md{ m_displayMeasure->getMonitors()->getMonitorData()[m_currMonitor] };

        updateSize(md.width, md.height);
        SetWindowPos(hWnd, HWND_TOP, md.x, md.y, md.width, md.height, 0);
    }
}

void Window::handleTrayMessage(HWND hWnd, WPARAM /*wParam*/, LPARAM lParam) {
    switch (LOWORD(lParam)) {
        case WM_MBUTTONUP:
            sendToBack();
            break;
        case WM_LBUTTONUP:
            SetForegroundWindow(hWnd);
            break;
        case WM_RBUTTONUP:
            createRClickMenu(hWnd);
            break;
        default:
            break;
    }
}

void Window::sendToBack() const {
    SetWindowPos(m_hWndMain, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

void GLAPIENTRY Window::GLMessageCallback(GLenum source, GLenum type, GLuint /*id*/,
                                          GLenum severity, GLsizei /*length*/,
                                          const GLchar * message, const void * /*userParam*/) {

    const auto& msg{ std::format("GL CALLBACK: {} source = {}, type = {}, severity = {} message = {}\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), source, type, severity, message)};
    std::cerr << msg << '\n';
}

void Window::updateSize(int width, int height) {
    m_width = width;
    m_height = height;
    m_retroGraph->updateWindowSize(width, height);
}

void Window::createWindow() {
    // Set and register the window class
    WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(IDI_APP_ICON));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(CreateSolidBrush(0x00000000));
    wc.lpszClassName = "RetroGraph";

    RGVERIFY(RegisterClassEx(&wc), "RegisterClassEx - failed");

    /* Build the window itself*/
    RGVERIFY(createHGLRC(), "Failed to create OpenGL Window");

    initOpenGL();

    ReleaseDC(m_hWndMain, m_hdc);
}

void Window::createTrayIcon() {
    m_tray.cbSize = sizeof(m_tray);
    m_tray.hIcon = LoadIcon(GetResourceHandle(), MAKEINTRESOURCE(IDI_APP_ICON));
    m_tray.hWnd = m_hWndMain;
    m_tray.uVersion = NOTIFYICON_VERSION_4;

    if constexpr (debugMode) {
        strcpy_s(m_tray.szTip, 128, "RetroGraph (Debug)");
    } else {
        strcpy_s(m_tray.szTip, 128, "RetroGraph");
    }

    m_tray.uCallbackMessage = WM_NOTIFY_RG_TRAY;
    m_tray.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    m_tray.uID = 101;

    RGVERIFY(Shell_NotifyIcon(NIM_ADD, &m_tray), "Failed to display notification icon");
}

void Window::initOpenGL() {
    glewInit();

    // Make dummy command line arguments for glutInit
    char* gargv[1] = { '\0' };
    int gargc{ 1 };
    glutInit(&gargc, gargv);

    // Enable debug info
    if constexpr (debugMode) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(GLMessageCallback, reinterpret_cast<void*>(this));

        // Filter messages about VBO memory usage
        constexpr GLuint ignoredMessageIds[] = { 131185 };
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE,
                              sizeof(ignoredMessageIds) / sizeof(GLuint), ignoredMessageIds, GL_FALSE);
    }

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    glEnable(GL_BLEND); // Alpha blending needed for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    std::cout << "Opengl version: " << glGetString(GL_VERSION) << '\n';
    std::cout << glGetString(GL_VENDOR) << ' ' << glGetString(GL_RENDERER) << '\n';
}

bool Window::createHGLRC() {
    /* Much of this function and its children are based on the work by
       Colt McAnlis. Code and explanations:
       http://nehe.gamedev.net/tutorial/fullscreen_antialiasing/16008/ */

    const auto windowName = []() {
        if constexpr (debugMode) {
            return std::string{ "RetroGraph (Debug)" };
        } else {
            return std::string{ "RetroGraph" };
        }
    }(); // Call immediately

    if (m_arbMultisampleSupported) {
        const DWORD exStyles = WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;

        // Create main window with clicking through to windows underneath, 
        // no taskbar display, and transparency
        m_hWndMain = CreateWindowEx(exStyles, "RetroGraph", windowName.c_str(),
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
    SetWindowLongPtr(m_hWndMain, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // Display window at the desktop layer on startup
    sendToBack();

    if (!m_hWndMain) {
        RGERROR("CreateWindowEx - failed");
        return false;
    }

    // Make window transparent via dwm
    const auto hRgn{ CreateRectRgn(0, 0, -1, -1) };
    DWM_BLURBEHIND bb = {0};
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(m_hWndMain, &bb);
    DeleteObject(hRgn);

    m_hdc = GetDC(m_hWndMain);
    if (!m_hdc) {
        DestroyWindow(m_hWndMain);
        m_hWndMain = nullptr;
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                                // Version number
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,                               // Color depth
        0, 0, 0, 0, 0, 0,                 // Color bits ignored
        8,                                // Alpha buffer
        0,                                // Shift bit ignored
        0,                                // No accumulation buffer
        0, 0, 0, 0,                       // Accumulation bits ignored
        24,                               // 16Bit Z-Buffer (Depth Buffer)
        8,                                // Some stencil buffer
        0,                                // No auxiliary buffer
        PFD_MAIN_PLANE,                   // Main drawing layer
        0,                                // Reserved
        0, 0, 0                           // layer masks ignored
    };

    // On the first run, create a regular window with the previous pfd
    // If multisampling is supported on the second run, use the alternate
    // pfd to create an anti-aliased window
    int pixelFormat;
    if (!m_arbMultisampleSupported) {
        pixelFormat = ChoosePixelFormat(m_hdc, &pfd);
        if (pixelFormat == 0) {
            ReleaseDC(m_hWndMain, m_hdc);
            DestroyWindow(m_hWndMain);
            RGERROR("ChoosePixelFormat - failed");
            return false;
        }
    } else {
        pixelFormat = m_arbMultisampleFormat;
    }

    // Set the pixel format for the window
    if (!SetPixelFormat(m_hdc, pixelFormat, &pfd)) {
        ReleaseDC(m_hWndMain, m_hdc);
        m_hdc = nullptr;
        DestroyWindow(m_hWndMain);
        m_hWndMain = nullptr;
        RGERROR("SetPixelFormat - failed");
        return false;
    }

    // Create an opengl context for the window
    m_hrc = wglCreateContext(m_hdc);
    if (!m_hrc){
        ReleaseDC(m_hWndMain, m_hdc);
        m_hdc = nullptr;
        DestroyWindow(m_hWndMain);
        m_hWndMain = nullptr;
        RGERROR("wglCreateContext - failed");
        return false;
    }

    // Make the context the current one for the window
    if (!wglMakeCurrent(m_hdc, m_hrc)) {
        wglDeleteContext(m_hrc);
        m_hrc = nullptr;
        ReleaseDC(m_hWndMain, m_hdc);
        m_hdc = nullptr;
        DestroyWindow(m_hWndMain);
        m_hWndMain = nullptr;

        RGERROR("Failed to make current context with wglMakeCurrent");
        return false;
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

bool Window::wglIisExtensionSupported(const std::string& extension) {
    std::string supported;

    // Try To Use wglGetExtensionStringARB on current dc, if possible
    const auto wglGetExtString{ reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB")) };

    if (wglGetExtString) {
        supported = wglGetExtString(wglGetCurrentDC());
    }

    // If that failed, try standard opengl extensions string
    if (supported.empty()) {
        supported = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    }

    // If that failed too, must be no extensions supported
    if (supported.empty()) {
        return false;
    }

    // Begin examination at start of string, increment by 1 on false match
    for (const char* p = supported.c_str(); ; p++) {
        // Advance p up to the next possible match
        p = strstr(p, extension.c_str());
        if (!p) {
            return false;
        }

        if ((p == supported.c_str() || p[-1] == ' ') &&
            (p[extension.size()] == '\0' || p[extension.size()] == ' ')) {

            return true;
        }
    }
}

bool Window::initMultisample() {
    if (!wglIisExtensionSupported("WGL_ARB_multisample")) {
        m_arbMultisampleSupported = false;
        return false;
    }

    const auto wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
    if (!wglChoosePixelFormatARB) {
        m_arbMultisampleSupported = false;
        return false;
    }

    // These attributes are the bits we want to test for in our sample.
    // Everything is pretty standard, the only one we want to
    // really focus on is the SAMPLE BUFFERS ARB and WGL SAMPLES.
    // These two are going to do the main testing for whether or not
    // we support multisampling on this hardware.
    int iAttributes[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 24,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 16,
        WGL_STENCIL_BITS_ARB, 0,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 8,
        0, 0
    };

    int pixelFormat;
    int valid;
    unsigned int numFormats;
    float fAttributes[] = { 0, 0 };

    // First we check to see if we can get a pixel format for 8 samples
    valid = wglChoosePixelFormatARB(m_hdc, iAttributes, fAttributes, 1,
                                    &pixelFormat, &numFormats);

    // If we returned true, and our format count is greater than 1
    if (valid && numFormats >= 1) {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return true;
    }

    // Our pixel format with 8 samples failed, test for 4 samples
    iAttributes[19] = 4;
    valid = wglChoosePixelFormatARB(m_hdc, iAttributes, fAttributes, 1,
                                    &pixelFormat,&numFormats);
    if (valid && numFormats >= 1) {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return true;
    }

    // Our pixel format with 2 samples failed, test for 2 samples
    iAttributes[19] = 2;
    valid = wglChoosePixelFormatARB(m_hdc, iAttributes, fAttributes, 1,
                                    &pixelFormat,&numFormats);
    if (valid && numFormats >= 1) {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return true;
    }

    return m_arbMultisampleSupported;
}

void Window::destroy() {
    // Release and zero the device context, openGL context and window handle
    if (m_hWndMain) {
        if (m_hdc) {
            wglMakeCurrent(m_hdc, nullptr);
            if (m_hrc) {
                wglDeleteContext(m_hrc);
                m_hrc = nullptr;
            }

            ReleaseDC(m_hWndMain, m_hdc);
            m_hdc = nullptr;
        }

        DestroyWindow(m_hWndMain);
        m_hWndMain = nullptr;
    }
}

} // namespace rg
