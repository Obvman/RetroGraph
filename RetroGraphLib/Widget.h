#pragma once

#include "stdafx.h"

#include "drawUtils.h"
// TODO integrate the FontManager observer

namespace rg {

enum class WidgetPosition : uint8_t {
    TOP_LEFT,
    TOP_MID,
    TOP_RIGHT,
    MID_LEFT,
    MID_MID,
    MID_RIGHT,
    BOT_LEFT,
    BOT_MID,
    BOT_RIGHT
};

enum WidgetType : size_t {
    ProcessRAM = 0U,
    ProcessCPU = 1U,
    Time = 2U,
    SystemStats = 3U,
    Music = 4U,
    CPUStats = 5U,
    HDD = 6U,
    Main = 7U,
    Graph = 8U,
    FPS = 9U,
    
    NumWidgets,
};

class FontManager;

class Widget {
public:
    Widget(const FontManager* fm, bool visibility) : 
        m_fontManager{ fm }, m_visible { visibility } { }
    virtual ~Widget() = default;

    /* Draws the widget. Must be overriden */
    virtual void draw() const = 0;

    /* Clears the widget's entire viewport */
    virtual void clear() const;

    /* Sets the viewport for the entire widget. Should be overriden
       for widgets with sub-viewports */
    virtual void setViewport(Viewport vp) { m_viewport = vp; }

    /* Turns on/off drawing of the widget */
    void setVisibility(bool b);
protected:
    bool m_visible;
    Viewport m_viewport{};
    const FontManager* m_fontManager;
};

} // namespace rg