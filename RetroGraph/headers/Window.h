#pragma once
#include <Windows.h>
#include <vector>

#include "GLShaderHandler.h"
#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "NetMeasure.h"
#include "SystemInfo.h"
#include "DriveMeasure.h"
#include "Renderer.h"

namespace rg {

class Window {
public:
    Window(HINSTANCE hInstance, const char* windowName,
           uint16_t width, uint16_t height, uint16_t startX, uint16_t startY);
    ~Window();

    /* Windows callback procedure for handling windows messages */
    static LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /* Makes the window visible */
    void show();

    /* Initialises the window's measurement components. Use before entering
       the update/draw loop */
    void init();

    /* Updates the window's components and passes tick information onto the
       components that have alternate timings */
    void update(uint32_t ticks);

    /* Draws the window's components*/
    void draw() const;

    /* Updates the OpenGL viewport when the window size changes */
    void updateSize(int width, int height);

    /* Returns the window handle*/
    HWND& getHWND() { return m_hWndMain; }

    /* Returns the OpenGL context for the window */
    HGLRC& getHRC() { return m_hrc; }

    /* Returns the device context for the window */
    HDC getHDC() { return GetWindowDC(m_hWndMain); }

    /* Returns the CPU measurement member */
    const CPUMeasure& getCPUMeasure() const { return m_cpuMeasure; }

    /* Returns the GPU measurement member */
    const GPUMeasure& getGPUMeasure() const { return m_gpuMeasure; }

    /* Returns the RAM measurement member */
    const RAMMeasure& getRAMMeasure() const { return m_ramMeasure; }
private:
    /* Initialises OpenGL settings and renderer */
    void initOpenGL();

    /* Releases the OpenGL context from the window */
    void releaseOpenGL();

    /* Creates a dummy window to test if multisampling is supported. If so,
       the dummy is destroyed and a new anti-aliased window is created */
    bool createHGLRC();

    bool initMultisample(PIXELFORMATDESCRIPTOR& pfd);

    /* Queries wgl to see if the given extension is supported on this device.
       Must be called with an OpenGL context active */
    bool wglIisExtensionSupported(const char *extension);

    /* Destroys the win32 window and releases resources */
    void destroy();

    const uint16_t m_width;
    const uint16_t m_height;
    uint16_t m_startPosX;
    uint16_t m_startPosY;

    CPUMeasure m_cpuMeasure;
    GPUMeasure m_gpuMeasure;
    RAMMeasure m_ramMeasure;
    ProcessMeasure m_processMeasure;
    DriveMeasure m_driveMeasure;
    NetMeasure m_netMeasure;
    SystemInfo m_systemInfo;

    Renderer m_renderer;

    HINSTANCE m_hInstance;
    WNDCLASSEX m_wc;
    HWND m_hWndMain;
    HDC m_hdc;
    HGLRC m_hrc;
    MSG m_msg;

    bool m_arbMultisampleSupported{ false };
    int32_t m_arbMultisampleFormat{ 0 };
    int32_t m_aaSamples{ 32 };

    GLShaderHandler m_shaders;
};

}