#pragma once

#include "Widget.h"
#include "drawUtils.h"

namespace rg {

class FontManager;
class CPUMeasure;
class RAMMeasure;
class NetMeasure;
class GPUMeasure;

class GraphWidget : public Widget {
public:
    GraphWidget(const FontManager* fontManager, const CPUMeasure* cpuMeasure,
                const RAMMeasure* ramMeasure, const NetMeasure* netMeasure,
                const GPUMeasure* gpuMeasure) :
        m_fontManager{ fontManager }, m_cpuMeasure{ cpuMeasure },
        m_ramMeasure{ ramMeasure }, m_netMeasure{ netMeasure },
        m_gpuMeasure{ gpuMeasure } {}

    virtual ~GraphWidget() noexcept = default;
    GraphWidget(const GraphWidget&) = delete;
    GraphWidget& operator=(const GraphWidget&) = delete;

    void draw() const override;

    void setViewport(Viewport vp);

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

    const CPUMeasure* m_cpuMeasure{ nullptr };
    const RAMMeasure* m_ramMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
    const GPUMeasure* m_gpuMeasure{ nullptr };
};

}
