#pragma once

#include "stdafx.h"

#include <cstdint>
#include <vector>
#include <Windows.h>
#include <GL/glew.h>

#include "FontManager.h"
#include "Widget.h"

namespace rg {

class RetroGraph;
class Window;
class GLShaderHandler;
class UserSettings;

/* In charge of rendering each component's data to the window */
class Renderer {
public:
    Renderer(const Window& w, const RetroGraph& _rg);
    ~Renderer() noexcept;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    /* Draws scene to the window */
    void draw(int ticks, const Window& window, int totalFPS) const;

    void updateWindowSize(int newWidth, int newHeight);

    void setWidgetVisibility(Widgets w, bool v);

    void updateObservers(const RetroGraph& rg);

    void setViewports(int windowWidth, int windowHeight);

private:
    Viewport calcViewport(WidgetPosition pos,
                          int windowWidth, int windowHeight,
                          std::vector<int>& positionFills);
    Viewport calcFPSViewport(WidgetPosition pos, int windowWidth, int windowHeight);

    HWND m_renderTargetHandle{ nullptr };

    FontManager m_fontManager;

    std::vector<std::unique_ptr<Widget>> m_widgets;

    auto createWidgets(const RetroGraph& _rg) -> decltype(m_widgets);
};

} // namespace rg
