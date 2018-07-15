#include "stdafx.h"

#include "Renderer.h"

#include "RetroGraph.h"
#include "Window.h"
#include "UserSettings.h"

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

namespace rg {

Renderer::Renderer(const Window& w, const RetroGraph& _rg) :
    m_renderTargetHandle{ w.getHwnd() },
    m_fontManager{ w.getHwnd(), w.getHeight() },
    m_widgets{ createWidgets(_rg) } {

    setViewports(w.getWidth(), w.getHeight());
}

Renderer::~Renderer() {
}

auto Renderer::createWidgets(const RetroGraph& _rg) -> decltype(m_widgets) {
    decltype(m_widgets) widgetList( Widgets::NumWidgets );

    const auto& s{ UserSettings::inst() };

    widgetList[Widgets::ProcessCPU] = std::make_unique<ProcessCPUWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::ProcessCPU)
    );
    widgetList[Widgets::ProcessRAM] = std::make_unique<ProcessRAMWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::ProcessRAM)
    );
    widgetList[Widgets::Time] = std::make_unique<TimeWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::Time)
    );
    widgetList[Widgets::SystemStats] = std::make_unique<SystemStatsWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::SystemStats)
    );
    widgetList[Widgets::Music] = std::make_unique<MusicWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::Music)
    );
    widgetList[Widgets::CPUStats] = std::make_unique<CPUStatsWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::CPUStats)
    );
    widgetList[Widgets::HDD] = std::make_unique<HDDWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::HDD)
    );
    widgetList[Widgets::Main] = std::make_unique<MainWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::Main)
    );
    widgetList[Widgets::Graph] = std::make_unique<GraphWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::Graph)
    );
    widgetList[Widgets::FPS] = std::make_unique<FPSWidget>(
        &m_fontManager, s.isVisible(Widgets::FPS)
    );
    widgetList[Widgets::NetStats] = std::make_unique<NetStatsWidget>(
        &m_fontManager, _rg, s.isVisible(Widgets::NetStats)
    );

    return widgetList;
}

void Renderer::draw(int ticks, const Window& window, int totalFPS) const {
    const auto& mainWidget{ dynamic_cast<MainWidget&>(*m_widgets[Widgets::Main]) };

    // Render the bulk of widgets at a low FPS to keep light on resources
    if (ticksMatchRate(ticks, 2U) || ticksMatchRate(ticks, totalFPS)) {

        HDC hdc = GetDC(window.getHwnd());
        wglMakeCurrent(hdc, window.getHGLRC());

        glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

        if (ticksMatchRate(ticks, 2U)) {
            for (const auto& widget : m_widgets) {
                if (widget.get() != &mainWidget)
                    widget->draw();
            }
        }

        // The main widget can have a higher framerate, so call every tick
        if (ticksMatchRate(ticks, totalFPS)) {
            if (mainWidget.needsDraw(ticks)) {
                mainWidget.draw();
            }
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
    m_widgets[w]->setVisibility(v);
}

void Renderer::updateObservers(const RetroGraph& rg) {
    for (const auto& widget : m_widgets)
        widget->updateObservers(rg);
}

void Renderer::setViewports(int windowWidth, int windowHeight) {
    // For positions that can contain multiple widgets, track how many widgets
    // will fill that position. Currently on the horizontal center positions
    // can contain multiple widgets
    std::vector<int> positionFills( 2, 0 );

    const auto& s{ UserSettings::inst() };

    for (auto i = size_t{ 0U }; i < Widgets::NumWidgets; ++i) {
        const Widgets w{ static_cast<Widgets>(i) };
        if (w == Widgets::FPS) {
            m_widgets[Widgets::FPS]->setViewport(calcFPSViewport(
                s.getWidgetPosition(Widgets::FPS), windowWidth, windowHeight
            ));
        } else {
            m_widgets[w]->setViewport(
                calcViewport(s.getWidgetPosition(w), windowWidth, windowHeight, positionFills)
            );
        }
    }

    if (positionFills[0] > 2) {
        fatalMessageBox("You put too many widgets in the top-middle area!");
    } else if (positionFills[1] > 2) {
        fatalMessageBox("You put too many widgets in the bottom-middle area!");
    }
}

/********************* Private Functions ********************/

Viewport Renderer::calcFPSViewport(WidgetPosition pos, int windowWidth, int windowHeight) {
    const auto widgetW{ windowWidth / 24 };
    const auto widgetH{ windowHeight / 24 };

    switch (pos) {
        case WidgetPosition::TOP_LEFT:
            return Viewport{ marginX, windowHeight - marginY - widgetH,
                             widgetW, widgetH };
        case WidgetPosition::TOP_RIGHT:
            return Viewport{ windowWidth - marginX - widgetW, windowHeight - marginY - widgetH,
                             widgetW, widgetH };
        case WidgetPosition::BOT_LEFT:
            return Viewport{ marginX, marginY, widgetW, widgetH };
        case WidgetPosition::BOT_RIGHT:
            return Viewport{ windowWidth - marginX - widgetW, marginY,
                             widgetW, widgetH };
        default:
            fatalMessageBox("Invalid position selected for FPS widget");
            break;
    }
    return Viewport{};
}

Viewport Renderer::calcViewport(WidgetPosition pos, int windowWidth, int windowHeight,
                                std::vector<int>& positionFills) {
    const auto widgetW{ windowWidth / 5 };
    const auto widgetH{ windowHeight / 6 };
    const auto sideWidgetH{ windowHeight / 2 };

    // For positions that fit multiple widgets, set the appropriate viewport
    // and increment the number of widgets in that position
    switch (pos) {
        case WidgetPosition::TOP_MID:
            ++positionFills[0];
            return Viewport{ 
                marginX + windowWidth/2 - widgetW + widgetW * (positionFills[0]-1),
                windowHeight - marginY - widgetH,
                widgetW,
                widgetH };
        case WidgetPosition::BOT_MID:
            ++positionFills[1];
            return Viewport{
                marginX + windowWidth/2 - widgetW + widgetW * (positionFills[1]-1),
                marginY,
                widgetW,
                widgetH };
        case WidgetPosition::MID_MID:
            return Viewport{ 
                marginX + windowWidth/2 - widgetW,
                windowHeight/2 - windowHeight/4,
                2 * widgetW,
                sideWidgetH };
        // Positions that only take a single widget
        case WidgetPosition::TOP_LEFT:
            return Viewport{  marginX, windowHeight - marginY -
                widgetH, widgetW, widgetH };
        case WidgetPosition::TOP_RIGHT:
            return Viewport{ windowWidth - widgetW - marginX,
                windowHeight - marginY - widgetH, widgetW, widgetH }; 
        case WidgetPosition::MID_LEFT:
            return Viewport{ marginX, windowHeight/2 -
                windowHeight/4, widgetW, sideWidgetH };
        case WidgetPosition::MID_RIGHT:
            return Viewport{ windowWidth - widgetW - marginX,
                windowHeight/2 - windowHeight/4, widgetW, sideWidgetH };
        case WidgetPosition::BOT_LEFT:
            return Viewport{ marginX, marginY, widgetW, widgetH };
        case WidgetPosition::BOT_RIGHT:
            return Viewport{ windowWidth - widgetW - marginX,
                    marginY, widgetW, widgetH };
        default:
            fatalMessageBox("Invalid widget position specified. Please check your config.ini file");
            return Viewport{};
    }
}

}
