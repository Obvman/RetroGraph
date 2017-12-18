#include "../headers/RetroGraph.h"

#include <iostream>

#include "../headers/drawUtils.h"

namespace rg {

RetroGraph::RetroGraph(HINSTANCE hInstance) :
    m_userSettings{ },
    m_window{ this, hInstance, m_userSettings.getStartupMonitor(), 
               m_userSettings.getClickthrough() },
    m_cpuMeasure{ m_userSettings },
    m_gpuMeasure{ m_userSettings },
    m_ramMeasure{ m_userSettings },
    m_netMeasure{ m_userSettings },
    m_processMeasure{ m_userSettings },
    m_driveMeasure{ std::make_unique<DriveMeasure>() },
    m_musicMeasure{ std::make_unique<MusicMeasure>(&m_processMeasure) },
    m_systemMeasure{ },
    m_renderer{ m_window, *this, m_userSettings } {

    g_widgetBGVisible = m_userSettings.getWidgetBackground();

    update(0);
    draw(0);
}

void RetroGraph::update(uint32_t ticks) {
    // Update with a tick offset so all measures don't update in the same
    // cycle

    auto i = uint32_t{ 0U };
    m_cpuMeasure.update(ticks + ++i);
    m_gpuMeasure.update(ticks + ++i);
    m_ramMeasure.update(ticks + ++i);
    m_netMeasure.update(ticks + ++i);
    m_processMeasure.update(ticks + ++i);
    if (m_driveMeasure) m_driveMeasure->update(ticks + ++i);
    if (m_musicMeasure) m_musicMeasure->update(ticks + ++i);
    m_systemMeasure.update(ticks + ++i);
}

void RetroGraph::draw(uint32_t ticks) const {
    constexpr auto framesPerSecond = uint32_t{ 2U };
    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

        HDC hdc = GetDC(m_window.getHwnd());
        wglMakeCurrent(hdc, m_window.getHGLRC());

        m_renderer.draw(ticks);

        SwapBuffers(hdc);
        ReleaseDC(m_window.getHwnd(), hdc);
    }
}

void RetroGraph::updateWindowSize(int32_t width, int32_t height) {
    m_renderer.updateWindowSize(width, height);
}

void RetroGraph::toggleTimeWidget() {
    m_timeWidgetEnabled = !m_timeWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::Time, m_timeWidgetEnabled);
}

void RetroGraph::toggleHDDWidget() {
    if (m_HDDWidgetEnabled) {
        // Disable the measure here to free up resources since it's not used anywhere else
        m_driveMeasure.reset();
    } else {
        m_driveMeasure = std::make_unique<DriveMeasure>();
    }
    m_HDDWidgetEnabled = !m_HDDWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::HDD, m_HDDWidgetEnabled);
}

void RetroGraph::toggleCPUStatsWidget() {
    m_cpuStatsWidgetEnabled = !m_cpuStatsWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::CPUStats, m_cpuStatsWidgetEnabled);
}

void RetroGraph::toggleCPUProcessWidget() {
    m_cpuProcessWidgetEnabled = !m_cpuProcessWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::ProcessCPU, m_cpuProcessWidgetEnabled);
}

void RetroGraph::toggleRAMProcessWidget() {
    m_ramProcessWidgetEnabled = !m_ramProcessWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::ProcessRAM, m_ramProcessWidgetEnabled);
}

void RetroGraph::toggleGraphWidget() {
    m_graphWidgetEnabled = !m_graphWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::Graph, m_graphWidgetEnabled);
}

void RetroGraph::toggleMainWidget() {
    m_mainWidgetEnabled = !m_mainWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::Main, m_mainWidgetEnabled);
}

void RetroGraph::toggleMusicWidget() {
    if (m_musicWidgetEnabled) {
        m_musicMeasure.reset();
    } else {
        m_musicMeasure = std::make_unique<MusicMeasure>(&m_processMeasure);
    }
    m_musicWidgetEnabled = !m_musicWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::Music, m_musicWidgetEnabled);
}

void RetroGraph::toggleSystemStatsWidget() {
    m_systemStatsWidgetEnabled = !m_systemStatsWidgetEnabled;
    m_renderer.setWidgetVisibility(Widget::SystemStats, m_systemStatsWidgetEnabled);
}

}
