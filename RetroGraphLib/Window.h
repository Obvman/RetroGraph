#pragma once

#include "stdafx.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

#include <vector>
#include <memory>

#include "Monitors.h"

namespace rg {

class RetroGraph;

class Window {
public:
    Window(RetroGraph* rg_, HINSTANCE hInstance, int startupMonitor,
           bool clickthrough);
    ~Window() noexcept;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    /* Just runs some experimental code */
    void runTest();

    /* Updates the OpenGL viewport when the window size changes */
    void updateSize(int width, int height);

    /* Window Proc that has access to this window class's members via lParam */
    LRESULT CALLBACK WndProc2(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    HWND getHwnd() const { return m_hWndMain; }
    HGLRC getHGLRC() const { return m_hrc; }

    bool isRunning() const { return m_running; }
private:
    /* Creates the window and the OpenGL context */
    void createWindow();

    /* Creates and displays program icon in the system tray */
    void createTrayIcon();

    void createRClickMenu(HWND hWnd);

    /* Moves the window to the specified monitor and resizes elements to fit
     * the monitor
     */
    void changeMonitor(HWND hWnd, int monIndex);

    /* Initialises OpenGL settings and renderer */
    void initOpenGL();

    /* Creates a dummy window to test if multisampling is supported. If so,
     * the dummy is destroyed and a new anti-aliased window is created 
     */
    bool createHGLRC();

    bool initMultisample();

    /* Queries wgl to see if the given extension is supported on this device.
     * Must be called with an OpenGL context active
     */
    bool wglIisExtensionSupported(const char *extension);

    /* Destroys the win32 window and releases resources for the purpose of
     * testing multisampling support
     */
    void destroy();

    /* Decides whether a click sets the dragging state based on the colour
     * of the click position. Clicking the background of the window does not
     * allow dragging, clicking on a coloured pixel allows dragging
     */
    void handleClick(DWORD clickX, DWORD clickY);

    /* Creates menu on interaction with notification tray icon */
    void handleTrayMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);

    /* Sends the window to the background layer (i.e. on the desktop) */
    void sendToBack() const;

    // Passes Window this pointer via userParam
    static void GLAPIENTRY GLMessageCallback(GLenum source, GLenum type, GLuint id,
                                             GLenum severity, GLsizei length,
                                             const GLchar* message, const void* userParam);

    bool m_running{ true };
    bool m_clickthrough{ true };

    const Monitors* m_monitors;
    RetroGraph* m_retroGraph;

    HWND m_hWndMain{ nullptr };
    HDC m_hdc{ };
    HGLRC m_hrc{ };

    NOTIFYICONDATA m_tray{ };
    bool m_dragging{ false };
    int m_currMonitor{ 0 };
    int m_width{ 0 };
    int m_height{ 0 };
    int m_startPosX{ 0 };
    int m_startPosY{ 0 };
    bool m_arbMultisampleSupported{ false };
    int m_arbMultisampleFormat{ 0 };
    int m_aaSamples{ 8 };
    HINSTANCE m_hInstance{ nullptr };
};

} // namespace rg
