#include "../headers/RetroGraph.h"

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
    m_driveMeasure{ },
    m_musicMeasure{ &m_processMeasure },
    m_systemMeasure{ },
    m_renderer{ m_window, *this } {

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
    m_driveMeasure.update(ticks + ++i);
    m_musicMeasure.update(ticks + ++i);
    m_systemMeasure.update(ticks + ++i);
}

void RetroGraph::draw(uint32_t ticks) const {
    constexpr auto framesPerSecond = uint32_t{ 2U };
    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

        const auto hdc{ m_window.startDraw() };

        m_renderer.draw(ticks);

        m_window.endDraw(hdc);
    }
}

void RetroGraph::updateWindowSize(int32_t width, int32_t height) {
    m_renderer.updateWindowSize(width, height);
}

}
