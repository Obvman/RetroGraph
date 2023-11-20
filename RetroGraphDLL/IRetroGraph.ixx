export module IRetroGraph;

import Widgets.WidgetType;

import std.core;

namespace rg {

export class IRetroGraph {
public:
    virtual ~IRetroGraph() = default;

    virtual void update(int ticks) = 0;
    virtual void draw(int ticks) const = 0;
    virtual bool isRunning() const = 0;
    virtual void toggleWidget(WidgetType w) = 0;
    virtual void updateWindowSize(int width, int height) = 0;
    virtual void shutdown() = 0;
};

}
