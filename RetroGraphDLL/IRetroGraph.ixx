export module IRetroGraph;

import FPSCounter;

import Widgets.WidgetType;

import std.core;

namespace rg {

export class IRetroGraph {
public:
    virtual ~IRetroGraph() = default;

    virtual void update() = 0;
    virtual void draw() const = 0;
    virtual bool isRunning() const = 0;
    virtual void reloadResources() = 0;
    virtual void toggleWidget(WidgetType w) = 0;
    virtual void updateWindowSize(int width, int height) = 0;
    virtual void shutdown() = 0;
    virtual FPSCounter& getFPSCounter() = 0;
};

}
