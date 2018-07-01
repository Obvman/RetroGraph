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
    void draw(uint32_t ticks, const Window& window, uint32_t totalFPS) const;

    void updateWindowSize(int32_t newWidth, int32_t newHeight);

    void setWidgetVisibility(Widgets w, bool v);

    void updateObservers(const RetroGraph& rg);

    GLuint getCircleList() const { return m_circleList; }
private:
    void setViewports(int32_t windowWidth, int32_t windowHeight);
    Viewport calcViewport(WidgetPosition pos,
                          int32_t windowWidth, int32_t windowHeight,
                          std::vector<int32_t>& positionFills);
    Viewport calcFPSViewport(WidgetPosition pos, int32_t windowWidth,
                             int32_t windowHeight);

    /* Create any glLists */
    void initLists();
    /* Fill VBOs with intial vertex data */
    void initVBOs();
    /* Compiles and retrieves uniform locations */
    void initShaders();

    HWND m_renderTargetHandle{ nullptr };

    FontManager m_fontManager;

    std::vector<std::unique_ptr<Widget>> m_widgets;

    GLuint m_circleList;

    // Shaders
    GLuint m_cpuGraphShader{ 0U };

    // Uniform location (UL) variables
    GLint m_graphAlphaLoc{ -1 };

    auto createWidgets(const RetroGraph& _rg) -> decltype(m_widgets);
};

} // namespace rg
