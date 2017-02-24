#pragma once
#include <Windows.h>
#include <vector>

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "SystemInfo.h"

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
    HWND& getHWND() { return m_hWnd; }

    /* Returns the OpenGL context for the window */
    HGLRC& getHRC() { return m_hrc; }

    /* Returns the device context for the window */
    HDC getHDC() { return GetWindowDC(m_hWnd); }

    /* Returns the CPU measurement member */
    const CPUMeasure& getCPUMeasure() const { return m_cpuMeasure; }

    /* Returns the GPU measurement member */
    const GPUMeasure& getGPUMeasure() const { return m_gpuMeasure; }

    /* Returns the RAM measurement member */
    const RAMMeasure& getRAMMeasure() const { return m_ramMeasure; }
private:
    /* Initialises OpenGL settings */
    void initOpenGL() const;
    /* Releases the OpenGL context from the window */
    void releaseOpenGL();

    /* Creates OpenGL context for this window */
    bool createHGLRC();

    const uint16_t m_width;
    const uint16_t m_height;
    uint16_t m_startPosX;
    uint16_t m_startPosY;

    CPUMeasure m_cpuMeasure;
    GPUMeasure m_gpuMeasure;
    RAMMeasure m_ramMeasure;
    ProcessMeasure m_processMeasure;
    SystemInfo m_systemInfo;

    WNDCLASSEX m_wc;
    HWND m_hWnd;
    HDC m_hdc;
    HGLRC m_hrc;
    MSG m_msg;
};

}