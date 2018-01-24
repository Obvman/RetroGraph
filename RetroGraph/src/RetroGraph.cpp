#include "../headers/RetroGraph.h"

#include <iostream>

#include "../headers/drawUtils.h"

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
    m_fpsLimiter{},
    m_renderer{ m_window, *this } {

    update(0);
    draw(0);
}

void RetroGraph::update(uint32_t ticks) {
    m_fpsLimiter.begin();

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
    const auto framesPerSecond = uint32_t{ (m_mainWidgetEnabled) ? 
        m_animationState->getAnimationFPS() : 2U };

    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

        HDC hdc = GetDC(m_window.getHwnd());
        wglMakeCurrent(hdc, m_window.getHGLRC());

        m_renderer.draw(ticks);

        SwapBuffers(hdc);
        ReleaseDC(m_window.getHwnd(), hdc);

        const auto fps{ m_fpsLimiter.end() };
        printf("%f\n", fps);
    }
}

void RetroGraph::updateWindowSize(int32_t width, int32_t height) {
    m_renderer.updateWindowSize(width, height);
}

void RetroGraph::toggleWidget(Widget w) {
    switch (w) {
        case Widget::Time:
            m_timeWidgetEnabled = !m_timeWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::Time, m_timeWidgetEnabled);
            break;
        case Widget::HDD:
            if (m_HDDWidgetEnabled) {
                // Disable the measure here to free up resources since 
                // it's not used anywhere else
                m_driveMeasure.reset();
            } else {
                m_driveMeasure = std::make_unique<DriveMeasure>();
            }
            m_HDDWidgetEnabled = !m_HDDWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::HDD, m_HDDWidgetEnabled);
            break;
        case Widget::CPUStats:
            m_cpuStatsWidgetEnabled = !m_cpuStatsWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::CPUStats, m_cpuStatsWidgetEnabled);
            break;
        case Widget::Graph:
            m_graphWidgetEnabled = !m_graphWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::Graph, m_graphWidgetEnabled);
            break;
        case Widget::Music:
            if (m_musicWidgetEnabled) {
                m_musicMeasure.reset();
            } else {
                m_musicMeasure = std::make_unique<MusicMeasure>(&m_processMeasure);
            }
            m_musicWidgetEnabled = !m_musicWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::Music, m_musicWidgetEnabled);
            break;
        case Widget::Main:
            if (m_mainWidgetEnabled) {
                m_animationState.reset();
            } else {
                m_animationState = std::make_unique<AnimationState>();
            }
            m_mainWidgetEnabled = !m_mainWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::Main, m_mainWidgetEnabled);
            break;
        case Widget::SystemStats:
            m_systemStatsWidgetEnabled = !m_systemStatsWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::SystemStats,
                                           m_systemStatsWidgetEnabled);
            break;
        case Widget::ProcessCPU:
            m_cpuProcessWidgetEnabled = !m_cpuProcessWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::ProcessCPU,
                                           m_cpuProcessWidgetEnabled);
            break;
        case Widget::ProcessRAM:
            m_ramProcessWidgetEnabled = !m_ramProcessWidgetEnabled;
            m_renderer.setWidgetVisibility(Widget::ProcessRAM,
                                           m_ramProcessWidgetEnabled);
            break;
    }
}

}
