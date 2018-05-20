#include "stdafx.h"

#include "Widget.h"

#include <string>
// #include <gl/glew.h>
// #include <gl/gl.h>

namespace rg {

void Widget::clear() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
}

void Widget::setVisibility(bool b) {
    m_visible = b;
    if (m_visible) {
        draw();
    } else {
        clear();
    }
}

// static
std::string Widget::widgetTypeToStr(WidgetType w) {
    std::string str{};

    switch (w) {
        case ProcessRAM:
            str = "ProcessRAM";
            break;
        case ProcessCPU:
            str = "ProcessCPU";
            break;
        case Time:
            str = "Time";
            break;
        case SystemStats:
            str = "SystemStats";
            break;
        case Music:
            str = "Music";
            break;
        case CPUStats:
            str = "CPUStats";
            break;
        case HDD:
            str = "HDD";
            break;
        case Main:
            str = "Main";
            break;
        case Graph:
            str = "Graph";
            break;
        case FPS:
            str = "FPS";
            break;
        default:
            str = "Invalid widget type";
            break;
    }

    return str;
}

}