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

class GPUGraphWidget : public Widget {
public:
    GPUGraphWidget(const FontManager* fontManager, const RetroGraph& rg, bool visible);

    void updateObservers(const RetroGraph& rg) override { m_gpuMeasure = &rg.getGPUMeasure(); };
    void draw() const override;
private:
    const GPUMeasure* m_gpuMeasure{ nullptr };

    VBOID m_gpuVBO;
};

class CPUGraphWidget : public Widget {
public:
    CPUGraphWidget(const FontManager* fontManager, const RetroGraph& rg, bool visible);

    void updateObservers(const RetroGraph& rg) override { m_cpuMeasure = &rg.getCPUMeasure(); };
    void draw() const override;
private:
    const CPUMeasure* m_cpuMeasure{ nullptr };

    VBOID m_cpuVBO;
};

class RAMGraphWidget : public Widget {
public:
    RAMGraphWidget(const FontManager* fontManager, const RetroGraph& rg, bool visible);

    void updateObservers(const RetroGraph& rg) override { m_ramMeasure = &rg.getRAMMeasure(); };
    void draw() const override;
private:
    const RAMMeasure* m_ramMeasure{ nullptr };

    VBOID m_ramVBO;
};

class NetGraphWidget : public Widget {
public:
    NetGraphWidget(const FontManager* fontManager, const RetroGraph& rg, bool visible);

    void updateObservers(const RetroGraph& rg) override { m_netMeasure = &rg.getNetMeasure(); };
    void draw() const override;
private:
    const NetMeasure* m_netMeasure{ nullptr };

    VBOID m_netUpVBO;
    VBOID m_netDownVBO;
};


} // namespace rg
