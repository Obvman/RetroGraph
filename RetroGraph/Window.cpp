#include "Window.h"

#include <GL/freeglut.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
#include <dwmapi.h>
#include <iostream>

#include "utils.h"
#include "colors.h"


namespace rg {

void drawBorder();

Window::Window(HINSTANCE hInstance, const char* windowName,
               uint16_t width, uint16_t height, uint16_t startX, uint16_t startY) :
    m_hInstance{ hInstance },
    m_width{ width },
    m_height{ height },
    m_startPosX{ startX },
    m_startPosY{ startY },
    m_cpuMeasure{ this, m_width, m_height / 3 },
    m_gpuMeasure{ },
    m_ramMeasure{ 0, 480, m_width / 3, m_height / 3 },
    m_processMeasure{ 0, 500, m_width / 3, m_height / 2 },
    m_systemInfo{} {

    WNDCLASSEX m_wc;
    memset(&m_wc, 0, sizeof(m_wc));
    m_wc.cbSize = sizeof(WNDCLASSEX);
    m_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    m_wc.style = CS_HREDRAW | CS_VREDRAW;
    m_wc.lpfnWndProc = Window::WndProc;
    m_wc.cbClsExtra  = 0;
    m_wc.cbWndExtra  = 0;
    m_wc.hInstance = m_hInstance;
    m_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
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

    ReleaseDC(m_hWnd, m_hdc);

    std::cout << m_systemInfo.getOSInfoStr() << '\n';
    std::cout << m_systemInfo.getGPUDescription() << '\n';
    std::cout << m_systemInfo.getCPUDescription() << '\n';
    std::cout << m_systemInfo.getRAMDescription() << '\n';
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
            //PostQuitMessage(0);
            //exit(0);
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

    m_processMeasure.init();

    draw();
}

void Window::update(uint32_t ticks) {
    if (ticks % 5 == 0) {
        m_cpuMeasure.update();
        m_gpuMeasure.update();
        m_ramMeasure.update();
    }

    // Only update processes each second
    if (ticks % 10 == 0) {
        m_processMeasure.update(ticks);
    }
}

void Window::draw() const {
    HDC hdc = GetDC(m_hWnd);
    wglMakeCurrent(hdc, m_hrc);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_cpuMeasure.draw();
    m_ramMeasure.draw();
    m_processMeasure.draw();

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
    char* gargv[1] = {""};
    int gargc{ 1 };
    glutInit(&gargc, gargv);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_MULTISAMPLE_ARB);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Window::releaseOpenGL() {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_hrc);
}

bool Window::createHGLRC() {
    m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, "RetroGraph", "RetroGraph",
                            WS_VISIBLE | WS_POPUP, m_startPosX, m_startPosY, m_width, m_height,
                            NULL, NULL, m_hInstance, NULL);

    #if (!_DEBUG)
    // Display window at the desktop layer on startup
    SetWindowPos(m_hWnd, HWND_BOTTOM, m_startPosX, m_startPosY, m_width, m_height, 0);
    #endif

    if(!m_hWnd) {
        fatalMessageBox("CreateWindowEx - failed");
    }

    // Make window transparent via dwm
    DWM_BLURBEHIND bb = {0};
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow(m_hWnd, &bb);

    m_hdc = GetDC(m_hWnd);
    if (m_hdc == 0) {
        DestroyWindow(m_hWnd);
        m_hWnd = 0;
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

    int PixelFormat;
    if (!m_arbMultisampleSupported) {
        PixelFormat = ChoosePixelFormat(m_hdc, &pfd);
        if (PixelFormat == 0) {
            ReleaseDC(m_hWnd, m_hdc);
            DestroyWindow(m_hWnd);
            fatalMessageBox("ChoosePixelFormat - failed");
            return false;
        }
    } else {
        std::cout << "Multisampling run begins\n";
        PixelFormat = m_arbMultisampleFormat;
    }


    if (!SetPixelFormat(m_hdc, PixelFormat, &pfd)) {
        ReleaseDC(m_hWnd, m_hdc);
        m_hdc = 0;
        DestroyWindow(m_hWnd);
        m_hWnd = 0;

        auto e = GetLastError();
        std::cout << "Error: " << e << '\n';

        fatalMessageBox("SetPixelFormat - failed");
        return false;
    }

    m_hrc = wglCreateContext(m_hdc);
    if (!m_hrc){
        ReleaseDC(m_hWnd, m_hdc);
        m_hdc = 0;
        DestroyWindow(m_hWnd);
        m_hWnd = 0;
        fatalMessageBox("wglCreateContext - failed");
        return false;
    }

    if (!wglMakeCurrent(m_hdc, m_hrc)) {
        wglDeleteContext(m_hrc);
        m_hrc = 0;
        ReleaseDC(m_hWnd, m_hdc);
        m_hdc = 0;
        DestroyWindow(m_hWnd);
        m_hWnd = 0;
        fatalMessageBox("Failed to make current context with wglMakeCurrent");
        return false;
    }

    if (!m_arbMultisampleSupported) {
        if (InitMultisample(m_hInstance, m_hWnd, pfd)) {
            DestroyWindowGL(this);
            return createHGLRC();
        }
    }

    return true;
}
bool Window::WGLisExtensionSupported(const char *extension)
{
    const size_t extlen = strlen(extension);
    const char *supported = NULL;

    // Try To Use wglGetExtensionStringARB On Current DC, If Possible
    PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

    if (wglGetExtString)
        supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

    // If That Failed, Try Standard Opengl Extensions String
    if (supported == NULL)
        supported = (char*)glGetString(GL_EXTENSIONS);

    // If That Failed Too, Must Be No Extensions Supported
    if (supported == NULL)
        return false;

    // Begin Examination At Start Of String, Increment By 1 On False Match
    for (const char* p = supported; ; p++)
    {
        // Advance p Up To The Next Possible Match
        p = strstr(p, extension);

        if (p == NULL)
            return false;															// No Match

                                                                                    // Make Sure That Match Is At The Start Of The String Or That
                                                                                    // The Previous Char Is A Space, Or Else We Could Accidentally
                                                                                    // Match "wglFunkywglExtension" With "wglExtension"

                                                                                    // Also, Make Sure That The Following Character Is Space Or NULL
                                                                                    // Or Else "wglExtensionTwo" Might Match "wglExtension"
        if ((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' '))
            return true;															// Match
    }
}
bool Window::InitMultisample(HINSTANCE hInstance,HWND hWnd,PIXELFORMATDESCRIPTOR pfd) {  
    // See If The String Exists In WGL!
    if (!WGLisExtensionSupported("WGL_ARB_multisample"))
    {
        m_arbMultisampleSupported=false;
        return false;
    }

    // Get Our Pixel Format
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 
        (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");	
    if (!wglChoosePixelFormatARB) 
    {
        m_arbMultisampleSupported=false;
        return false;
    }

    // Get Our Current Device Context
    HDC hDC = GetDC(hWnd);

    int		pixelFormat;
    int		valid;
    UINT	numFormats;
    float	fAttributes[] = {0,0};

    // These Attributes Are The Bits We Want To Test For In Our Sample
    // Everything Is Pretty Standard, The Only One We Want To 
    // Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
    // These Two Are Going To Do The Main Testing For Whether Or Not
    // We Support Multisampling On This Hardware.
    int iAttributes[] =
    {
        WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
        WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB,24,
        WGL_ALPHA_BITS_ARB,8,
        WGL_DEPTH_BITS_ARB,16,
        WGL_STENCIL_BITS_ARB,0,
        WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
        WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
        WGL_SAMPLES_ARB,16,
        0,0
    };

    // First We Check To See If We Can Get A Pixel Format For 4 Samples
    valid = wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats);

    // If We Returned True, And Our Format Count Is Greater Than 1
    if (valid && numFormats >= 1)
    {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return m_arbMultisampleSupported;
    }

    // Our Pixel Format With 4 Samples Failed, Test For 2 Samples
    iAttributes[19] = 2;
    valid = wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats);
    if (valid && numFormats >= 1)
    {
        m_arbMultisampleSupported = true;
        m_arbMultisampleFormat = pixelFormat;
        return m_arbMultisampleSupported;
    }

    // Return The Valid Format
    return m_arbMultisampleSupported;
}

bool Window::DestroyWindowGL(Window* window) {							// Destroy The OpenGL Window & Release Resources
    if (m_hWnd != 0)												// Does The Window Have A Handle?
    {	
        if (m_hdc != 0)											// Does The Window Have A Device Context?
        {
            wglMakeCurrent(m_hdc, 0);							// Set The Current Active Rendering Context To Zero
            if (m_hrc != 0)										// Does The Window Have A Rendering Context?
            {
                wglDeleteContext(m_hrc);							// Release The Rendering Context
                m_hrc = 0;										// Zero The Rendering Context

            }
            ReleaseDC(m_hWnd, m_hdc);						// Release The Device Context
            m_hdc = 0;											// Zero The Device Context
        }
        DestroyWindow(m_hWnd);									// Destroy The Window
        m_hWnd = 0;												// Zero The Window Handle
    }

    return true;														// Return True
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
