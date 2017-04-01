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
    GraphWidget() = default;
    virtual ~GraphWidget() = default;
    GraphWidget(const GraphWidget&) = delete;
    GraphWidget& operator=(const GraphWidget&) = delete;

    virtual void draw() const override;

    void init(const FontManager* fontManager, const CPUMeasure* cpuMeasure,
              const RAMMeasure* ramMeasure, const NetMeasure* netMeasure,
              const GPUMeasure* gpuMeasure);

    void setViewport(Viewport vp);

private:
    void drawCpuGraph() const;
    void drawRamGraph() const;
    void drawNetGraph() const;
    void drawGpuGraph() const;

    const FontManager* m_fontManager;
    Viewport m_cpuGraphVP;
    Viewport m_ramGraphVP;
    Viewport m_netGraphVP;
    Viewport m_gpuGraphVP;

    const CPUMeasure* m_cpuMeasure;
    const RAMMeasure* m_ramMeasure;
    const NetMeasure* m_netMeasure;
    const GPUMeasure* m_gpuMeasure;
};

}
