#pragma once

#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;
class RAMMeasure;
class NetMeasure;
class GPUMeasure;

class GraphWidget {
public:
    GraphWidget(const FontManager* fontManager, const CPUMeasure* cpuMeasure,
                const RAMMeasure* ramMeasure, const NetMeasure* netMeasure,
                const GPUMeasure* gpuMeasure, bool visible) :
        m_fontManager{ fontManager }, m_visible{ visible }, 
        m_cpuMeasure{ cpuMeasure }, m_ramMeasure{ ramMeasure },
        m_netMeasure{ netMeasure }, m_gpuMeasure{ gpuMeasure } {}

    ~GraphWidget() noexcept = default;
    GraphWidget(const GraphWidget&) = delete;
    GraphWidget& operator=(const GraphWidget&) = delete;

    void draw() const;

    void setViewport(Viewport vp);

    void setVisible(bool b) { m_visible = b; }
private:
    void drawCpuGraph() const;
    void drawRamGraph() const;
    void drawNetGraph() const;
    void drawGpuGraph() const;

    const FontManager* m_fontManager{ nullptr };
    Viewport m_viewport{ };
    Viewport m_cpuGraphVP{ };
    Viewport m_ramGraphVP{ };
    Viewport m_netGraphVP{ };
    Viewport m_gpuGraphVP{ };
    bool m_visible{ true };

    const CPUMeasure* m_cpuMeasure{ nullptr };
    const RAMMeasure* m_ramMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
    const GPUMeasure* m_gpuMeasure{ nullptr };
};

}
