export module Widgets.Widget;

import IRetroGraph; // Reverse Dependency
import Utils;

import Rendering.DrawUtils;
import Rendering.FontManager;
import Rendering.GLListContainer;
import Rendering.Viewport;

import Widgets.WidgetPosition;
import Widgets.WidgetType;

import std.core;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

export enum class ContainerType {
    VFill, // Widgets fill up space top to bottom
    HFill, // Widgets fill up space left to right
    Single, // Only one widget can exist in this container
    Mini,  // Mini-sized container. Only one widget can exist in this container
};

export class Widget {
public:
    Widget(const FontManager* fm, bool visibility) : 
        m_fontManager{ fm }, m_visible { visibility } { }
    virtual ~Widget() = default;
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(Widget&&) = delete;

    /* Draws the widget. Must be overriden */
    virtual void draw() const = 0;

    /* Clears the widget's entire viewport */
    virtual void clear() const;

    /* Sets the viewport for the entire widget. Should be overriden
       for widgets with sub-viewports */
    virtual void setViewport(const Viewport& vp) { m_viewport = vp; }
    
    /* Sets the observer pointers of the widget */
    virtual void updateObservers(const IRetroGraph&) { }

    /* Turns on/off drawing of the widget */
    void setVisibility(bool b);

    bool isVisible() const { return m_visible; }

    static std::string widgetTypeToStr(WidgetType w);

    const Viewport& getViewport() const { return m_viewport; }

protected:
    Viewport m_viewport{};
    const FontManager* m_fontManager;

private:
    bool m_visible;
};

/* Represents a distinct region of the window in which Widgets can be placed 
 * Can contain multiple widgets, and is responsible for positioning each widget
 * withing the container, and drawing them
 */
export class WidgetContainer {
public:
    explicit WidgetContainer(WidgetPosition p);
    ~WidgetContainer() = default;

    void draw() const;
    void clear() const;
    bool isVisible() const;

    void setViewport(int windowWidth, int windowHeight, WidgetPosition pos);
    void addChild(Widget* child);
    void removeChild(Widget* child);
    void clearChildren() { m_children.clear(); }
    void setType(ContainerType t) { m_type = t; }
    void resetType() { m_type = getFillTypeFromPosition(m_pos); }

private:
    void setChildViewports(const Viewport & vp, WidgetPosition pos);
    Viewport getMiniViewport(WidgetPosition pos) const;
    ContainerType getFillTypeFromPosition(WidgetPosition p) const;

    std::vector<Widget*> m_children;
    Viewport m_viewport;
    WidgetPosition m_pos;
    ContainerType m_type;
};

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
std::string Widget::widgetTypeToStr(WidgetType w) {
    switch (w) {
        case WidgetType::ProcessRAM:
            return "ProcessRAM";
            break;
        case WidgetType::ProcessCPU:
            return "ProcessCPU";
            break;
        case WidgetType::Time:
            return "Time";
            break;
        case WidgetType::SystemStats:
            return "SystemStats";
            break;
        case WidgetType::Music:
            return "Music";
            break;
        case WidgetType::CPUStats:
            return "CPUStats";
            break;
        case WidgetType::HDD:
            return "HDD";
            break;
        case WidgetType::Main:
            return "Main";
            break;
        case WidgetType::CPUGraph:
            return "CPUGraph";
            break;
        case WidgetType::RAMGraph:
            return "RAMGraph";
            break;
        case WidgetType::NetGraph:
            return "NetGraph";
            break;
        case WidgetType::FPS:
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
                GLListContainer::inst().drawTopAndBottomSerifs();

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
                RGASSERT(numWidgets <= 1, "Attempting to place too many widgets in the same position");
                m_children[i]->setViewport(m_viewport);
                break;
            case ContainerType::Mini:
                RGASSERT(numWidgets <= 1, "Attempting to place too many widgets in the same position");
                m_children[i]->setViewport(getMiniViewport(pos));
                break;
            default:
                RGERROR("Invalid ContainerType specified");
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
            RGERROR("Invalid mini viewport position");
    }
    return Viewport{};
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
