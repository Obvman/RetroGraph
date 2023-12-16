export module Widgets.Widget;

import Utils;

import RG.Rendering;

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

    /* Returns whether the widget has changed and needs to be drawn again */
    virtual bool needsRedraw() const { return m_needsRedraw; }

    /* Clears the widget's entire viewport */
    virtual void clear() const;

    /* Sets the viewport for the entire widget. Should be overriden
       for widgets with sub-viewports */
    virtual void setViewport(const Viewport& vp) { m_viewport = vp; }

    const Viewport& getViewport() const { return m_viewport; }

    /* Mark the widget to be redrawn */
    void invalidate() { m_needsRedraw = true; }
    void validate() { m_needsRedraw = false; }

protected:
    Viewport m_viewport;
    const FontManager* m_fontManager;

private:
    bool m_needsRedraw;
};

} // namespace rg
