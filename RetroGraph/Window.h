#pragma once
#include <windows.h>
#include <vector>

namespace rg {

class Window {
public:
    Window(HINSTANCE hInstance, const char* windowName);
    ~Window();

    static LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void show();

    void draw(const std::vector<float>& floats) const;
    void updateSize(int width, int height);

    HWND& getHWND() { return m_hWnd; }
    HGLRC& getHRC() { return m_hrc; }

private:
    void initOpenGL() const;
    void releaseOpenGL();

    bool createHGLRC();

    WNDCLASSEX m_wc;
    HWND m_hWnd;
    HDC m_hdc;
    HGLRC m_hrc;
    MSG m_msg;

};

}