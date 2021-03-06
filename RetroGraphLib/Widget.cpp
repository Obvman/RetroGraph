#include "stdafx.h"

#include "Widget.h"

#include "ListContainer.h"
#include "utils.h"

#include <string>
#include <algorithm>
// #include <gl/glew.h>
// #include <gl/gl.h>

namespace rg {

void Widget::clear() const {
    viewport(m_viewport);
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
std::string Widget::widgetTypeToStr(Widgets w) {
    switch (w) {
        case ProcessRAM:
            return "ProcessRAM";
            break;
        case ProcessCPU:
            return "ProcessCPU";
            break;
        case Time:
            return "Time";
            break;
        case SystemStats:
            return "SystemStats";
            break;
        case Music:
            return "Music";
            break;
        case CPUStats:
            return "CPUStats";
            break;
        case HDD:
            return "HDD";
            break;
        case Main:
            return "Main";
            break;
        case CPUGraph:
            return "CPUGraph";
            break;
        case RAMGraph:
            return "RAMGraph";
            break;
        case NetGraph:
            return "NetGraph";
            break;
        case FPS:
            return "FPS";
            break;
        default:
            return "Invalid widget type";
            break;
    }
}

WidgetContainer::WidgetContainer(WidgetPosition p)
    : m_children{}
    , m_viewport{}
    , m_pos{ p }
    , m_type{ getFillTypeFromPosition(p) } {

}

bool WidgetContainer::isVisible() const {
    return !m_children.empty() 
        && std::any_of(m_children.cbegin(), m_children.cend(), [](const auto* w) { return w->isVisible(); });
}

void WidgetContainer::clear() const {
    viewport(m_viewport);
    scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
}

void rg::WidgetContainer::setViewport(int windowWidth, int windowHeight, WidgetPosition pos) {
    const auto widgetW{ windowWidth / 5 };
    const auto widgetH{ windowHeight / 6 };
    const auto sideWidgetH{ windowHeight / 2 };

    m_pos = pos;

    Viewport vp{};
    switch (pos) {
        case WidgetPosition::BOT_LEFT:
            vp = { marginX, marginY, widgetW, widgetH };
            break;
        case WidgetPosition::BOT_MID:
            vp = { marginX + windowWidth/2 - widgetW, marginY, widgetW * 2, widgetH };
            break;
        case WidgetPosition::BOT_RIGHT:
            vp = { windowWidth - widgetW - marginX, marginY, widgetW, widgetH };
            break;
        case WidgetPosition::MID_LEFT:
            vp = { marginX, windowHeight/2 - windowHeight/4, widgetW, sideWidgetH };
            break;
        case WidgetPosition::MID_MID:
            vp = { marginX + windowWidth/2 - widgetW, windowHeight/2 - windowHeight/4, 2 * widgetW, sideWidgetH };
            break;
        case WidgetPosition::MID_RIGHT:
            vp = { windowWidth - widgetW - marginX, windowHeight/2 - windowHeight/4, widgetW, sideWidgetH };
            break;
        case WidgetPosition::TOP_LEFT:
            vp = {  marginX, windowHeight - marginY - widgetH, widgetW, widgetH };
            break;
        case WidgetPosition::TOP_MID:
            vp = { marginX + windowWidth/2 - widgetW, windowHeight - marginY - widgetH, widgetW * 2, widgetH };
            break;
        case WidgetPosition::TOP_RIGHT:
            vp = { windowWidth - widgetW - marginX, windowHeight - marginY - widgetH, widgetW, widgetH }; 
            break;
        default:
            break;
    }
    setChildViewports(vp, pos);
}

void WidgetContainer::addChild(Widget * child) {
    m_children.push_back(child);
}

void WidgetContainer::removeChild(Widget * child) {
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
}

void WidgetContainer::draw() const {
    if (isVisible()) {
        clear();
        for (const auto* w : m_children) {
            if (w->isVisible()) {
                viewport(w->getViewport());
                rg::drawWidgetBackground();
                ListContainer::inst().drawTopAndBottomSerifs();

                w->draw();
            }
        }
    }
}

void WidgetContainer::setChildViewports(const Viewport & vp, WidgetPosition pos) {
    m_viewport = vp;

    const auto numWidgets{ static_cast<GLint>(m_children.size()) };
    for (auto i = int{ 0 }; i < numWidgets; ++i) {
        switch (m_type) {
            case ContainerType::VFill: {
                const auto height{ m_viewport.height / static_cast<GLint>(m_children.size()) };
                const auto startY{ m_viewport.y + (height * (numWidgets - 1 - i)) };
                m_children[i]->setViewport(Viewport{ m_viewport.x, startY, m_viewport.width, height });
                break;
            }
            case ContainerType::HFill: {
                const auto width{ m_viewport.width / static_cast<GLint>(m_children.size()) };
                const auto startX{ m_viewport.x + (width * i) };
                m_children[i]->setViewport(Viewport{ startX, m_viewport.y, width, m_viewport.height });
                break;
            }
            case ContainerType::Single:
                if (numWidgets > 1)
                    fatalMessageBox("Attempting to place too many widgets in the same position");
                m_children[i]->setViewport(m_viewport);
                break;
            case ContainerType::Mini:
                if (numWidgets > 1)
                    fatalMessageBox("Attempting to place too many widgets in the same position");
                m_children[i]->setViewport(getMiniViewport(pos));
                break;
            default:
                fatalMessageBox("Invalid ContainerType specified");
                break;
        }
    }
}

Viewport WidgetContainer::getMiniViewport(WidgetPosition pos) const {
    const auto miniWidgetWidth{ m_viewport.width / 5 };
    const auto miniWidgetHeight{ m_viewport.height / 4 };

    switch (pos) {
        case WidgetPosition::BOT_LEFT:
            return { m_viewport.x,
                     m_viewport.y,
                     miniWidgetWidth,
                     miniWidgetHeight };
        case WidgetPosition::BOT_RIGHT:
            return { m_viewport.x + m_viewport.width - miniWidgetWidth,
                     m_viewport.y,
                     miniWidgetWidth,
                     miniWidgetHeight };
        case WidgetPosition::TOP_LEFT:
            return { m_viewport.x,
                     m_viewport.y + m_viewport.height - miniWidgetHeight,
                     miniWidgetWidth,
                     miniWidgetHeight };
        case WidgetPosition::TOP_RIGHT:
            return { m_viewport.x + m_viewport.width - miniWidgetWidth,
                     m_viewport.y + m_viewport.height - miniWidgetHeight,
                     miniWidgetWidth,
                     miniWidgetHeight };
        default:
            fatalMessageBox("Invalid mini viewport position");
            return m_viewport;
    }
}

ContainerType WidgetContainer::getFillTypeFromPosition(WidgetPosition p) const {
    if (p == WidgetPosition::TOP_MID || p == WidgetPosition::BOT_MID)
        return ContainerType::HFill;
    if (p == WidgetPosition::MID_LEFT || p == WidgetPosition::MID_RIGHT)
        return ContainerType::VFill;
    else 
        return ContainerType::Single;
}

}