#pragma once

#include "stdafx.h"

#include "drawUtils.h"
#include "Widget.h"
#include "RetroGraph.h"
#include "VBOController.h"

// #include <GL/GL.h>

namespace rg {

class CPUMeasure;
class RAMMeasure;
class NetMeasure;
class GPUMeasure;

class GraphWidget : public Widget {
public:
    GraphWidget(const FontManager* fontManager,
                const RetroGraph& rg, bool visible);

    ~GraphWidget() noexcept = default;
    GraphWidget(const GraphWidget&) = delete;
    GraphWidget& operator=(const GraphWidget&) = delete;
    GraphWidget(GraphWidget&&) = delete;
    GraphWidget& operator=(GraphWidget&&) = delete;

    void updateObservers(const RetroGraph& rg) override;
    void draw() const override;
    void setViewport(Viewport vp) override;

private:
    void drawCpuGraph() const;
    void drawRamGraph() const;
    void drawNetGraph() const;
    void drawGpuGraph() const;

    Viewport m_cpuGraphVP{ };
    Viewport m_ramGraphVP{ };
    Viewport m_netGraphVP{ };
    Viewport m_gpuGraphVP{ };

    const CPUMeasure* m_cpuMeasure{ nullptr };
    const RAMMeasure* m_ramMeasure{ nullptr };
    const NetMeasure* m_netMeasure{ nullptr };
    const GPUMeasure* m_gpuMeasure{ nullptr };

    VBOID m_cpuVBO;
    VBOID m_ramVBO;
    VBOID m_netUpVBO;
    VBOID m_netDownVBO;
    VBOID m_gpuVBO;
};

} // namespace rg
