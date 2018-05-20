#include "stdafx.h"

#include "RetroGraph.h"

#include <iostream>

#include "drawUtils.h"
#include "Renderer.h"
#include "Measure.h"
#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "NetMeasure.h"
#include "ProcessMeasure.h"
#include "DriveMeasure.h"
#include "MusicMeasure.h"
#include "SystemMeasure.h"
#include "AnimationState.h"

namespace rg {

RetroGraph::RetroGraph(HINSTANCE hInstance) :
    m_window{ this, hInstance, 
              std::get<int32_t>(UserSettings::inst().getVal("Window.Monitor")), 
              std::get<bool>(UserSettings::inst().getVal("Window.ClickThrough")) },
    m_widgetVisibilities( Widgets::NumWidgets ),
    m_cpuMeasure{ std::make_unique<CPUMeasure>() },
    m_gpuMeasure{ std::make_unique<GPUMeasure>() },
    m_ramMeasure{ std::make_unique<RAMMeasure>() },
    m_netMeasure{ std::make_unique<NetMeasure>() },
    m_processMeasure{ std::make_unique<ProcessMeasure>() },
    m_driveMeasure{ std::make_unique<DriveMeasure>() },
    m_musicMeasure{  std::make_unique<MusicMeasure>(m_processMeasure) },
    m_systemMeasure{ std::make_unique<SystemMeasure>() },
    m_animationState{ std::make_unique<AnimationState>() },
    m_renderer{ std::make_unique<Renderer>(m_window, *this) },
    m_dependencyMap{
        { Measures::AnimationState, { Widgets::Main } },
        { Measures::MusicMeasure,   { Widgets::Music } },
        { Measures::ProcessMeasure, { Widgets::Music, Widgets::ProcessCPU, Widgets::ProcessRAM } },
        { Measures::SystemMeasure,  { Widgets::SystemStats } },
        { Measures::NetMeasure,     { Widgets::Graph, Widgets::Time, Widgets::SystemStats } },
        { Measures::CPUMeasure,     { Widgets::CPUStats, Widgets::Graph, Widgets::SystemStats, Widgets::Time } },
        { Measures::GPUMeasure,     { Widgets::Graph } },
        { Measures::RAMMeasure,     { Widgets::Graph } },
        { Measures::DriveMeasure,   { Widgets::HDD } },
    } 
    {

    for (auto i = size_t{ 0U }; i < Widgets::NumWidgets; ++i) {
        m_widgetVisibilities[i] = UserSettings::inst().isVisible(static_cast<Widgets>(i));
    }

    checkDependencies();

    update(0);
    draw(0);
}

RetroGraph::~RetroGraph() {

}

void RetroGraph::update(uint32_t ticks) {

    // Update with a tick offset so all measures don't update in the same
    // cycle and spike the CPU
    auto i = uint32_t{ 0U };
    if (m_cpuMeasure) m_cpuMeasure->update(ticks + ++i);
    if (m_gpuMeasure) m_gpuMeasure->update(ticks + ++i);
    if (m_ramMeasure) m_ramMeasure->update(ticks + ++i);
    if (m_netMeasure) m_netMeasure->update(ticks + ++i);
    if (m_processMeasure) m_processMeasure->update(ticks + ++i);
    if (m_driveMeasure) m_driveMeasure->update(ticks + ++i);
    if (m_musicMeasure) m_musicMeasure->update(ticks + ++i);
    if (m_systemMeasure) m_systemMeasure->update(ticks + ++i);
    if (m_animationState) m_animationState->update(ticks);
}

void RetroGraph::draw(uint32_t ticks) const {
    // If the main widget is running, draw at it's target FPS,
    // Otherwise, we don't have to waste cycles swapping buffers when
    // the other measures update twice a second, so just draw at 2 FPS
    const auto framesPerSecond = uint32_t{ (m_widgetVisibilities[Widgets::Main]) ? 
        m_animationState->getAnimationFPS() : 2U };

    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

        HDC hdc = GetDC(m_window.getHwnd());
        wglMakeCurrent(hdc, m_window.getHGLRC());

        m_renderer->draw(ticks);

        SwapBuffers(hdc);
        ReleaseDC(m_window.getHwnd(), hdc);

        FPSLimiter::inst().end();
        FPSLimiter::inst().begin();
    }
}

void RetroGraph::updateWindowSize(int32_t width, int32_t height) {
    m_renderer->updateWindowSize(width, height);
}

void RetroGraph::needsRedraw() const {
     m_renderer->needsRedraw();
}

void RetroGraph::toggleWidget(Widgets w) {
    m_widgetVisibilities[w] = !m_widgetVisibilities[w];
    checkDependencies();
    m_renderer->setWidgetVisibility(w, m_widgetVisibilities[w]);
}

void RetroGraph::checkDependencies() {
    // Check dependant measures, if theres a measure that isn't being used by any
    // Widget, we can disable it. If a disabled measure needs to be used by a widget,
    // reenable it
    for (const auto&[measure, widgets] : m_dependencyMap) {
        bool allDependentWidgetsDisabled{ true };

        for (const auto& w : widgets) {
            if (m_widgetVisibilities[w]) {
                allDependentWidgetsDisabled = false;
                break;
            }
        }

        switch (measure) {
            case Measures::AnimationState:
                if (m_animationState) {
                    if (allDependentWidgetsDisabled) {
                        m_animationState.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_animationState = std::make_unique<AnimationState>();
                }
                break;
            case Measures::DriveMeasure:
                if (m_driveMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_driveMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_driveMeasure = std::make_unique<DriveMeasure>();
                }
                break;
            case Measures::MusicMeasure:
                if (m_musicMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_musicMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_musicMeasure = std::make_unique<MusicMeasure>(m_processMeasure);
                }
                break;
            case Measures::NetMeasure:
                if (m_netMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_netMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_netMeasure = std::make_unique<NetMeasure>();
                }
                break;
            case Measures::SystemMeasure:
                if (m_systemMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_systemMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_systemMeasure = std::make_unique<SystemMeasure>();
                }
                break;
            case Measures::ProcessMeasure:
                if (m_processMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_processMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_processMeasure = std::make_unique<ProcessMeasure>();
                }
                break;
            case Measures::RAMMeasure:
                if (m_ramMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_ramMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_ramMeasure = std::make_unique<RAMMeasure>();
                }
                break;
            case Measures::CPUMeasure:
                if (m_cpuMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_cpuMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_cpuMeasure = std::make_unique<CPUMeasure>();
                }
                break;
            case Measures::GPUMeasure:
                if (m_gpuMeasure) {
                    if (allDependentWidgetsDisabled) {
                        m_gpuMeasure.reset();
                    }
                } else if (!allDependentWidgetsDisabled) {
                    m_gpuMeasure = std::make_unique<GPUMeasure>();
                }
                break;
            default:
                break;
        }
    }

    // Most Widgets store observers to Measures, this updates their value
    // in case of destruction/construction of Measures
    m_renderer->updateObservers(*this);
}

}
