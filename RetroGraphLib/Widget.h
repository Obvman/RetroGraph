#pragma once

#include "drawUtils.h"
// TODO merge widgets.h into this file
// TODO integrate the FontManager observer

namespace rg {

class Widget {
public:
    Widget() : m_visible{ true } {}
    Widget(bool visibility) : m_visible{ visibility } {}
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
    Viewport m_viewport;
};

}