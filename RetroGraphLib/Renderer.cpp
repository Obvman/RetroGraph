#include "stdafx.h"

#include "Renderer.h"

#include "RetroGraph.h"
#include "Window.h"

#include "TimeWidget.h"
#include "HDDWidget.h"
#include "CPUStatsWidget.h"
#include "ProcessCPUWidget.h"
#include "ProcessRAMWidget.h"
#include "GraphWidget.h"
#include "SystemStatsWidget.h"
#include "MainWidget.h"
#include "MusicWidget.h"
#include "FPSWidget.h"
#include "NetStatsWidget.h"

#include "colors.h"
#include "utils.h"
#include "units.h"
#include "GLShaders.h"

//import UserSettings;

namespace rg {

auto Renderer::createWidgetContainers() const {
    decltype(m_widgetContainers) widgetContainerList;
    for (auto i = int{ 0 }; i < static_cast<int>(WidgetPosition::NUM_POSITIONS); ++i)
        widgetContainerList.emplace_back(std::make_unique<WidgetContainer>(static_cast<WidgetPosition>(i)));

    return widgetContainerList;
}

auto Renderer::createWidgets(const RetroGraph& _rg) const {
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

Renderer::Renderer(const Window& w, const RetroGraph& _rg)
    : m_renderTargetHandle{ w.getHwnd() }
    , m_fontManager{ w.getHwnd(), w.getHeight() }
    , m_widgets{ createWidgets(_rg) }
    , m_widgetContainers{ createWidgetContainers() } {

    setViewports(w.getWidth(), w.getHeight());
}

Renderer::~Renderer() {
}

void Renderer::draw(int ticks, const Window& window, int totalFPS) const {
    if (ticksMatchRate(ticks, 2U) || ticksMatchRate(ticks, totalFPS)) {

        HDC hdc = GetDC(window.getHwnd());
        wglMakeCurrent(hdc, window.getHGLRC());

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
        ReleaseDC(window.getHwnd(), hdc);

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

void Renderer::updateObservers(const RetroGraph& rg) {
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
