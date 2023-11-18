export module Rendering.Renderer;

import IRetroGraph; // Reverse Dependency

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.WidgetType;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

/* In charge of rendering each component's data to the window */
export class Renderer {
public:
    Renderer(HWND window, int width, int height, const IRetroGraph& _rg);
    __declspec(dllexport) ~Renderer() noexcept;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    /* Draws scene to the window */
    void draw(int ticks, HWND window, HGLRC hrc, int totalFPS) const;

    void updateWindowSize(int newWidth, int newHeight);

    void setWidgetVisibility(WidgetType w, bool v);

    void updateObservers(const IRetroGraph& rg);

    void setViewports(int windowWidth, int windowHeight);

private:
    auto createWidgets(const IRetroGraph& _rg) const;
    auto createWidgetContainers() const;

    HWND m_renderTargetHandle{ nullptr };
    FontManager m_fontManager;
    std::vector<std::unique_ptr<Widget>> m_widgets;
    std::vector<std::unique_ptr<WidgetContainer>> m_widgetContainers;
};

}
