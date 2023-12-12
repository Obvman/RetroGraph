export module Widgets.Widget;

import Utils;

import Rendering.FontManager;
import Rendering.Viewport;

import std.memory;

namespace rg {

export class Widget {
public:
    Widget(const FontManager* fm)
        : m_viewport{}
        , m_fontManager{ fm }
        , m_needsRedraw{ true } {
    }
    virtual ~Widget();
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(Widget&&) = delete;

    /* Draws the widget. Must be overriden */
    virtual void draw() const = 0;

    /* Clears the widget's entire viewport */
    virtual void clear() const;

    /* Mark the widget to be redrawn */
    virtual void invalidate() { m_needsRedraw = true; }

    /* Sets the viewport for the entire widget. Should be overriden
       for widgets with sub-viewports */
    virtual void setViewport(const Viewport& vp) { m_viewport = vp; }

    const Viewport& getViewport() const { return m_viewport; }

protected:
    Viewport m_viewport;
    const FontManager* m_fontManager;

private:
    bool m_needsRedraw;
};

} // namespace rg
