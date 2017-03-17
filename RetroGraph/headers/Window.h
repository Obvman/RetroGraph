#pragma once
#include <Windows.h>
#include <vector>

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "NetMeasure.h"
#include "SystemInfo.h"
#include "DriveMeasure.h"
#include "Renderer.h"
#include "UserSettings.h"

namespace rg {

class Window {
public:
    Window(HINSTANCE hInstance);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    /* Initialises the window's measurement components. Use before entering
       the update/draw loop */
    void init();

    /* Updates the window's components and passes tick information onto the
       components that have alternate timings */
    void update(uint32_t ticks);

    /* Draws the window's components*/
    void draw(uint32_t ticks) const;

    /* Updates the OpenGL viewport when the window size changes */
    void updateSize(int width, int height);

    /* Window Proc that has access to this window class's members */
    LRESULT CALLBACK WndProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
    /* Creates the window and the OpenGL context */
    void createWindow();

    /* Initialises OpenGL settings and renderer */
    void initOpenGL();

    /* Releases the OpenGL context from the window */
    void releaseOpenGL();

    /* Creates a dummy window to test if multisampling is supported. If so,
       the dummy is destroyed and a new anti-aliased window is created */
    bool createHGLRC();

    bool initMultisample();

    /* Queries wgl to see if the given extension is supported on this device.
       Must be called with an OpenGL context active */
    bool wglIisExtensionSupported(const char *extension);

    /* Destroys the win32 window and releases resources */
    void destroy();

    /* Creates the popup menu in response to a right click*/
    static void createRClickMenu(HWND hWnd, DWORD cursorX, DWORD cursorY);

    UserSettings m_userSettings;

    HINSTANCE m_hInstance;
    bool m_dragging;
    uint32_t m_width;
    uint32_t m_height;
    int32_t m_startPosX;
    int32_t m_startPosY;
    bool m_arbMultisampleSupported;
    int32_t m_arbMultisampleFormat;
    int32_t m_aaSamples;

    std::vector<std::unique_ptr<Measure>> m_measures;
    SystemInfo m_systemInfo;

    Renderer m_renderer;

    WNDCLASSEX m_wc;
    HWND m_hWndMain;
    HDC m_hdc;
    HGLRC m_hrc;
    MSG m_msg;
};

}