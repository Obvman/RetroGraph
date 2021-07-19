#pragma once

#include "stdafx.h"

#include "drawUtils.h"

namespace rg {

enum class WidgetPosition : int {
    TOP_LEFT  = 0,
    TOP_MID   = 1,
    TOP_RIGHT = 2,
    MID_LEFT  = 3,
    MID_MID   = 4,
    MID_RIGHT = 5,
    BOT_LEFT  = 6,
    BOT_MID   = 7,
    BOT_RIGHT = 8,

    NUM_POSITIONS
};

enum class Widgets : int { // Must be consecutive integers!
    ProcessCPU  = 0,
    ProcessRAM  = 1,
    Time        = 2,
    SystemStats = 3,
    Music       = 4,
    CPUStats    = 5,
    HDD         = 6,
    Main        = 7,
    FPS         = 8,
    NetStats    = 9,
    CPUGraph    = 10,
    RAMGraph    = 11,
    NetGraph    = 12,
    GPUGraph    = 13,
    
    NumWidgets
};

enum class ContainerType {
    VFill, // Widgets fill up space top to bottom
    HFill, // Widgets fill up space left to right
    Single, // Only one widget can exist in this container
    Mini,  // Mini-sized container. Only one widget can exist in this container
};

class FontManager;
class RetroGraph;

class Widget {
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
    virtual void updateObservers(const RetroGraph&) { }

    /* Turns on/off drawing of the widget */
    void setVisibility(bool b);

    bool isVisible() const { return m_visible; }

    static std::string widgetTypeToStr(Widgets w);

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
class WidgetContainer {
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

} // namespace rg