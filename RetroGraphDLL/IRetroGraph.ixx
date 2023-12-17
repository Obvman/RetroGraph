export module IRetroGraph;

import RG.Widgets;

import std.core;

namespace rg {

export class IRetroGraph {
public:
    virtual ~IRetroGraph() = default;

    virtual void run() = 0;
    virtual void reloadResources() = 0;
    virtual void toggleWidget(WidgetType w) = 0;
    virtual void toggleWidgetBackgroundVisible() = 0;
    virtual void updateWindowSize(int width, int height) = 0;
    virtual void shutdown() = 0;
};

}
