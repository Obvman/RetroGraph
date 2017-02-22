#pragma once
#include <windows.h>
#include <vector>

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"

namespace rg {

class Window {
public:
    Window(HINSTANCE hInstance, const char* windowName,
           uint16_t width, uint16_t height, uint16_t startX, uint16_t startY);
    ~Window();

    static LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void show();

    void init();
    void update(uint16_t ticks);
    void draw() const;
    void updateSize(int width, int height);

    HWND& getHWND() { return m_hWnd; }
    HGLRC& getHRC() { return m_hrc; }

    const CPUMeasure& getCPUMeasure() const { return m_cpuMeasure; }
    const GPUMeasure& getGPUMeasure() const { return m_gpuMeasure; }
    const RAMMeasure& getRAMMeasure() const { return m_ramMeasure; }

private:
    void initOpenGL() const;
    void releaseOpenGL();

    bool createHGLRC();

    const uint16_t m_width;
    const uint16_t m_height;
    uint16_t m_startPosX;
    uint16_t m_startPosY;

    CPUMeasure m_cpuMeasure;
    GPUMeasure m_gpuMeasure;
    RAMMeasure m_ramMeasure;
    ProcessMeasure m_processMeasure;

    WNDCLASSEX m_wc;
    HWND m_hWnd;
    HDC m_hdc;
    HGLRC m_hrc;
    MSG m_msg;
};

}