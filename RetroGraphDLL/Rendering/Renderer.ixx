module;

export module Rendering.Renderer;

import Colors;
import FPSLimiter;
import IRetroGraph; // Reverse Dependency
import Units;
import UserSettings;
import Utils;

import Rendering.FontManager;
import Rendering.GLShader;

import Widgets.CPUStatsWidget;
import Widgets.FPSWidget;
import Widgets.GraphWidget;
import Widgets.HDDWidget;
import Widgets.Widget;
import Widgets.MainWidget;
import Widgets.MusicWidget;
import Widgets.NetStatsWidget;
import Widgets.ProcessCPUWidget;
import Widgets.ProcessRAMWidget;
import Widgets.SystemStatsWidget;
import Widgets.TimeWidget;
import Widgets.WidgetDefines;

import std.core;

import "WindowsHeaders.h";

namespace rg {

export class Renderer;

/* In charge of rendering each component's data to the window */
class Renderer {
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

    void setWidgetVisibility(Widgets w, bool v);

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

auto Renderer::createWidgetContainers() const {
    decltype(m_widgetContainers) widgetContainerList;
    for (auto i = int{ 0 }; i < static_cast<int>(WidgetPosition::NUM_POSITIONS); ++i)
        widgetContainerList.emplace_back(std::make_unique<WidgetContainer>(static_cast<WidgetPosition>(i)));

    return widgetContainerList;
}

auto Renderer::createWidgets(const IRetroGraph& _rg) const {
    decltype(m_widgets) widgetList( static_cast<int>(Widgets::NumWidgets) );

    const auto& s{ UserSettings::inst() };

    widgetList[static_cast<int>(Widgets::ProcessCPU)] = std::make_unique<ProcessCPUWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::ProcessCPU)
    );
    widgetList[static_cast<int>(Widgets::ProcessRAM)] = std::make_unique<ProcessRAMWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::ProcessRAM)
    );
    widgetList[static_cast<int>(Widgets::Time)] = std::make_unique<TimeWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::Time)
    );
    widgetList[static_cast<int>(Widgets::SystemStats)] = std::make_unique<SystemStatsWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::SystemStats)
    );
    widgetList[static_cast<int>(Widgets::Music)] = std::make_unique<MusicWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::Music)
    );
    widgetList[static_cast<int>(Widgets::CPUStats)] = std::make_unique<CPUStatsWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::CPUStats)
    );
    widgetList[static_cast<int>(Widgets::HDD)] = std::make_unique<HDDWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::HDD)
    );
    widgetList[static_cast<int>(Widgets::Main)] = std::make_unique<MainWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::Main)
    );
    widgetList[static_cast<int>(Widgets::FPS)] = std::make_unique<FPSWidget>(
        &m_fontManager, s.isVisible(Widgets::FPS)
    );
    widgetList[static_cast<int>(Widgets::NetStats)] = std::make_unique<NetStatsWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::NetStats)
    );
    widgetList[static_cast<int>(Widgets::CPUGraph)] = std::make_unique<CPUGraphWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::CPUGraph)
    );
    widgetList[static_cast<int>(Widgets::RAMGraph)] = std::make_unique<RAMGraphWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::RAMGraph)
    );
    widgetList[static_cast<int>(Widgets::NetGraph)] = std::make_unique<NetGraphWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::NetGraph)
    );
    widgetList[static_cast<int>(Widgets::GPUGraph)] = std::make_unique<GPUGraphWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::GPUGraph)
    );

    return widgetList;
}

Renderer::Renderer(HWND window, int width, int height, const IRetroGraph& _rg)
    : m_renderTargetHandle{ window }
    , m_fontManager{ window, height }
    , m_widgets{ createWidgets(_rg) }
    , m_widgetContainers{ createWidgetContainers() } {

    setViewports(width, height);
}

Renderer::~Renderer() {
}

void Renderer::draw(int ticks, HWND window, HGLRC hrc, int totalFPS) const {
    if (ticksMatchRate(ticks, 2U) || ticksMatchRate(ticks, totalFPS)) {

        HDC hdc = GetDC(window);
        wglMakeCurrent(hdc, hrc);

        glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

        // Render the bulk of widgets at a low FPS to keep light on resources
        if (ticksMatchRate(ticks, 2U)) {
            for (const auto& widgetContainer : m_widgetContainers)
                widgetContainer->draw();

        } else if (ticksMatchRate(ticks, totalFPS)) {
            // The main widget can have a higher framerate, so call every tick
            const auto& mainWidget{ dynamic_cast<MainWidget&>(*m_widgets[static_cast<int>(Widgets::Main)]) };
            const auto& mainWidgetContainer{ m_widgetContainers[static_cast<int>(WidgetPosition::MID_MID)] };
            if (mainWidget.needsDraw(ticks))
                mainWidgetContainer->draw();
        }

        SwapBuffers(hdc);
        ReleaseDC(window, hdc);

        FPSLimiter::inst().end();
        FPSLimiter::inst().begin();
    }
}

void Renderer::updateWindowSize(int newWidth, int newHeight) {
    setViewports(newWidth, newHeight);
    m_fontManager.refreshFonts(newHeight);
}

void Renderer::setWidgetVisibility(Widgets w, bool v) {
    m_widgets[static_cast<int>(w)]->setVisibility(v);

    // Update the children of the widget container
    const auto pos{ UserSettings::inst().getWidgetPosition(w) };
    if (v) {
        m_widgetContainers[static_cast<int>(pos)]->addChild(m_widgets[static_cast<int>(w)].get());
    } else {
        m_widgetContainers[static_cast<int>(pos)]->removeChild(m_widgets[static_cast<int>(w)].get());
    }
}

void Renderer::updateObservers(const IRetroGraph& rg) {
    std::for_each(m_widgets.begin(), m_widgets.end(), [&rg](const auto& w) { w->updateObservers(rg); });
}

void Renderer::setViewports(int windowWidth, int windowHeight) {
    // Update child widgets. Their position may have changed, so they'll need to go to the correct container
    std::for_each(m_widgetContainers.begin(), m_widgetContainers.end(), [](const auto& wc) { wc->clearChildren(); });

    for (auto i = int{ 0 }; i < static_cast<int>(Widgets::NumWidgets); ++i) {
        const auto widgetType{ static_cast<Widgets>(i) };
        const auto widgetPos{ UserSettings::inst().getWidgetPosition(widgetType) };
        const auto& widget{ m_widgets[static_cast<int>(widgetType)] };
        WidgetContainer& container{ *m_widgetContainers[static_cast<int>(widgetPos)] };

        if (widgetType == Widgets::FPS)
            container.setType(ContainerType::Mini);
        else
            container.resetType();

        if (widget->isVisible())
            container.addChild(widget.get());
    }

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetPosition::NUM_POSITIONS); ++i) {
        m_widgetContainers[i]->setViewport(windowWidth, windowHeight, static_cast<WidgetPosition>(i));
        m_widgetContainers[i]->clear();
    }
}

}
