export module RG.Widgets:WidgetContainer;

import :Widget;
import :WidgetPosition;

import RG.Application;
import RG.Rendering;

import std.core;

namespace rg {

export enum class ContainerType {
    VFill, // Widgets fill up space top to bottom
    HFill, // Widgets fill up space left to right
    Single, // Only one widget can exist in this container
    Mini,  // Mini-sized container. Only one widget can exist in this container
};

/* Represents a distinct region of the window in which Widgets can be placed 
 * Can contain multiple widgets, and is responsible for positioning each widget
 * withing the container, and drawing them
 */
export class WidgetContainer {
public:
    explicit WidgetContainer(WidgetPosition p, bool drawBackground);
    ~WidgetContainer();

    void draw();
    bool isVisible() const;

    void setViewport(int windowWidth, int windowHeight, WidgetPosition pos);
    void addChild(Widget* child);
    void removeChild(Widget* child);
    void clearChildren() { m_children.clear(); }
    void setType(ContainerType t) { m_type = t; }
    void resetType() { m_type = getFillTypeFromPosition(m_pos); }
    void setDrawBackground(bool drawBackground) { m_drawBackground = drawBackground; }

private:
    void invalidate();
    void setChildViewports(const Viewport & vp, WidgetPosition pos);
    Viewport getMiniViewport(WidgetPosition pos) const;
    ContainerType getFillTypeFromPosition(WidgetPosition p) const;

    std::vector<Widget*> m_children;
    Viewport m_viewport;
    WidgetPosition m_pos;
    ContainerType m_type;
    bool m_drawBackground;
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
};

} // namespace rg
