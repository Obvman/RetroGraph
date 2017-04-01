#include "../headers/RetroGraph.h"

namespace rg {

RetroGraph::RetroGraph(HINSTANCE hInstance) :
    m_userSettings{ },
    m_window{ this, hInstance, m_userSettings.getStartupMonitor() },
    m_cpuMeasure{ },
    m_gpuMeasure{ },
    m_ramMeasure{ },
    m_netMeasure{ m_userSettings },
    m_processMeasure{ },
    m_driveMeasure{ },
    m_musicMeasure{ &m_processMeasure },
    m_systemMeasure{ },
    m_renderer{ m_window, *this } {

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
    const auto hdc{ m_window.startDraw() };

    m_renderer.draw(ticks);

    m_window.endDraw(hdc);
}

void RetroGraph::updateWindowSize(int32_t width, int32_t height) {
    m_renderer.updateWindowSize(width, height);
}

}
