#include "stdafx.h"

#include "RetroGraph.h"

#include <iostream>

#include "drawUtils.h"

namespace rg {

RetroGraph::RetroGraph(HINSTANCE hInstance) :
    m_window{ this, hInstance, 
              std::get<int32_t>(UserSettings::inst().getVal("Window.Monitor")), 
              std::get<bool>(UserSettings::inst().getVal("Window.ClickThrough")) },
    m_cpuMeasure{},
    m_gpuMeasure{},
    m_ramMeasure{},
    m_netMeasure{},
    m_processMeasure{},
    m_driveMeasure{ std::make_unique<DriveMeasure>() },
    m_musicMeasure{ std::make_unique<MusicMeasure>(&m_processMeasure) },
    m_systemMeasure{ },
    m_animationState{ std::make_unique<AnimationState>() },
    m_renderer{ m_window, *this },
    m_dependencyMap{
        { "AnimationState", WidgetType::Main },
        { "MusicMeasure", WidgetType::Music },

        { "ProcessMeasure", WidgetType::Music },
        { "ProcessMeasure", WidgetType::ProcessCPU },
        { "ProcessMeasure", WidgetType::ProcessRAM },

        { "SystemMeasure", WidgetType::SystemStats },

        { "CPUMeasure", WidgetType::CPUStats },
        { "CPUMeasure", WidgetType::Graph },
        { "CPUMeasure", WidgetType::SystemStats },
        { "CPUMeasure", WidgetType::Time },

        { "GPUMeasure", WidgetType::Graph },
        { "RAMMeasure", WidgetType::Graph },

        { "NetMeasure", WidgetType::Graph },
        { "NetMeasure", WidgetType::Time },
        { "NetMeasure", WidgetType::SystemStats },

        { "DriveMeasure", WidgetType::HDD },
    } {

    update(0);
    draw(0);

    /*for (const auto&[measure, widget] : m_dependencyMap) {
        std::cout << measure << " => ";
        switch (widget) {
            case WidgetType::Main:
                std::cout << "MainWidget" << '\n';
                break;
            case WidgetType::Music:
                std::cout << "MusicWidget" << '\n';
                break;
            case WidgetType::ProcessCPU:
                std::cout << "ProcessCPUWidget" << '\n';
                break;
            case WidgetType::ProcessRAM:
                std::cout << "ProcessRAMWidget" << '\n';
                break;
            case WidgetType::SystemStats:
                std::cout << "SystemStatsWidget" << '\n';
                break;
            case WidgetType::Time:
                std::cout << "TimeWidget" << '\n';
                break;
            case WidgetType::CPUStats:
                std::cout << "CPUStatsWidget" << '\n';
                break;
            case WidgetType::Graph:
                std::cout << "GraphWidget" << '\n';
                break;
            case WidgetType::HDD:
                std::cout << "HDDWidget" << '\n';
                break;
        }
    }*/
}

void RetroGraph::update(uint32_t ticks) {

    // Update with a tick offset so all measures don't update in the same
    // cycle and spike the CPU
    auto i = uint32_t{ 0U };
    m_cpuMeasure.update(ticks + ++i);
    m_gpuMeasure.update(ticks + ++i);
    m_ramMeasure.update(ticks + ++i);
    m_netMeasure.update(ticks + ++i);
    m_processMeasure.update(ticks + ++i);
    if (m_driveMeasure) m_driveMeasure->update(ticks + ++i);
    if (m_musicMeasure) m_musicMeasure->update(ticks + ++i);
    m_systemMeasure.update(ticks + ++i);
    if (m_animationState) m_animationState->update(ticks);
}

void RetroGraph::draw(uint32_t ticks) const {
    // If the main widget is running, draw at it's target FPS,
    // Otherwise, we don't have to waste cycles swapping buffers when
    // the other measures update twice a second, so just draw at 2 FPS
    const auto framesPerSecond = uint32_t{ (m_mainWidgetEnabled) ? 
        m_animationState->getAnimationFPS() : 2U };

    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

        HDC hdc = GetDC(m_window.getHwnd());
        wglMakeCurrent(hdc, m_window.getHGLRC());

        m_renderer.draw(ticks);

        SwapBuffers(hdc);
        ReleaseDC(m_window.getHwnd(), hdc);

        FPSLimiter::inst().end();
        FPSLimiter::inst().begin();
    }
}

void RetroGraph::updateWindowSize(int32_t width, int32_t height) {
    m_renderer.updateWindowSize(width, height);
}

void RetroGraph::toggleWidget(WidgetType w) {
    switch (w) {
        case WidgetType::Time:
            m_timeWidgetEnabled = !m_timeWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::Time, m_timeWidgetEnabled);
            break;
        case WidgetType::HDD:
            if (m_HDDWidgetEnabled) {
                // Disable the measure here to free up resources since 
                // it's not used anywhere else
                m_driveMeasure.reset();
            } else {
                m_driveMeasure = std::make_unique<DriveMeasure>();
            }
            m_HDDWidgetEnabled = !m_HDDWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::HDD, m_HDDWidgetEnabled);
            break;
        case WidgetType::CPUStats:
            m_cpuStatsWidgetEnabled = !m_cpuStatsWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::CPUStats, m_cpuStatsWidgetEnabled);
            break;
        case WidgetType::Graph:
            m_graphWidgetEnabled = !m_graphWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::Graph, m_graphWidgetEnabled);
            break;
        case WidgetType::Music:
            if (m_musicWidgetEnabled) {
                m_musicMeasure.reset();
            } else {
                m_musicMeasure = std::make_unique<MusicMeasure>(&m_processMeasure);
            }
            m_musicWidgetEnabled = !m_musicWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::Music, m_musicWidgetEnabled);
            break;
        case WidgetType::Main:
            if (m_mainWidgetEnabled) {
                m_animationState.reset();
            } else {
                m_animationState = std::make_unique<AnimationState>();
            }
            m_mainWidgetEnabled = !m_mainWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::Main, m_mainWidgetEnabled);
            break;
        case WidgetType::SystemStats:
            m_systemStatsWidgetEnabled = !m_systemStatsWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::SystemStats,
                                           m_systemStatsWidgetEnabled);
            break;
        case WidgetType::ProcessCPU:
            m_cpuProcessWidgetEnabled = !m_cpuProcessWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::ProcessCPU,
                                           m_cpuProcessWidgetEnabled);
            break;
        case WidgetType::ProcessRAM:
            m_ramProcessWidgetEnabled = !m_ramProcessWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::ProcessRAM,
                                           m_ramProcessWidgetEnabled);
            break;
        case WidgetType::FPS:
            m_fpsWidgetEnabled = !m_fpsWidgetEnabled;
            m_renderer.setWidgetVisibility(WidgetType::FPS,
                                           m_fpsWidgetEnabled);
            break;
    }

    // Check dependant measures, if theres a measure that isn't being used by any
    // Widget, we can disable it. If a disabled measure needs to be used by a widget,
    // reenable it
    //for ()
    for (const auto&[measureName, widget] : m_dependencyMap) {
        (void)widget;
        std::cout << "Measure: " << measureName << '\n';
    }
}

}
