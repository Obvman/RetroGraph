#pragma once

#define NOMINMAX
#include <Windows.h>
#include <vector>

#include "Renderer.h"
#include "UserSettings.h"
#include "SystemMeasure.h"
#include "Monitors.h"

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "NetMeasure.h"
#include "DriveMeasure.h"
#include "MusicMeasure.h"

namespace rg {

class RetroGraph;

class Window {
public:
    Window(RetroGraph* rg_, HINSTANCE hInstance, int32_t startupMonitor);
    ~Window() noexcept;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    /* Updates the OpenGL viewport when the window size changes */
    void updateSize(int32_t width, int32_t height);

    HDC startDraw() const;
    void endDraw(HDC hdc) const;

    /* Window Proc that has access to this window class's members via lParam */
    LRESULT CALLBACK WndProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    int32_t getWidth() const { return m_width; }
    int32_t getHeight() const { return m_height; }
    HWND getHwnd() const { return m_hWndMain; }

    bool isRunning() const { return m_running; }
private:
    /* Creates the window and the OpenGL context */
    void createWindow();

    /* Creates and displays program icon in the system tray */
    void createTrayIcon();

    /* Moves the window to the specified monitor and resizes elements to fit
     * the monitor
     */
    void changeMonitor(HWND hWnd, uint32_t monIndex);

    /* Initialises OpenGL settings and renderer */
    void initOpenGL();

    /* Creates a dummy window to test if multisampling is supported. If so,
       the dummy is destroyed and a new anti-aliased window is created */
    bool createHGLRC();

    bool initMultisample();

    /* Queries wgl to see if the given extension is supported on this device.
       Must be called with an OpenGL context active */
    bool wglIisExtensionSupported(const char *extension);

    /* Destroys the win32 window and releases resources */
    void destroy();

    /* Decides whether a click sets the dragging state based on the colour
     * of the click position. Clicking the background of the window does not
     * allow dragging, clicking on a coloured pixel allows dragging
     */
    void handleClick(DWORD clickX, DWORD clickY);

    /* Creates menu on interaction with notification tray icon
     */
    void handleTrayMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);

    bool m_running{ true };

    Monitors m_monitors;
    RetroGraph* m_retroGraph;

    WNDCLASSEX m_wc{ };
    HWND m_hWndMain{ nullptr };
    HDC m_hdc{ };
    HGLRC m_hrc{ };
    MSG m_msg{ };

    NOTIFYICONDATA m_tray{ };
    bool m_dragging{ false };
    int32_t m_currMonitor{ 0 };
    int32_t m_width{ 0 };
    int32_t m_height{ 0 };
    int32_t m_startPosX{ 0 };
    int32_t m_startPosY{ 0 };
    bool m_arbMultisampleSupported{ false };
    int32_t m_arbMultisampleFormat{ 0 };
    int32_t m_aaSamples{ 8 };
    HINSTANCE m_hInstance{ nullptr };

};

}
