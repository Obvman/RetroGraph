export module Widgets.GraphWidget;

import IRetroGraph; // Reverse Dependency

import Measures.CPUMeasure;
import Measures.GPUMeasure;
import Measures.NetMeasure;
import Measures.RAMMeasure;

import Rendering.FontManager;
import Rendering.VBO;

import Widgets.Widget;

namespace rg {

export class GPUGraphWidget : public Widget {
public:
    GPUGraphWidget(const FontManager* fontManager, const IRetroGraph& rg, bool visible);

    void updateObservers(const IRetroGraph& rg) override { m_gpuMeasure = &rg.getGPUMeasure(); };
    void draw() const override;
private:
    const GPUMeasure* m_gpuMeasure{ nullptr };

    VBOID m_gpuVBO;
};

export class CPUGraphWidget : public Widget {
public:
    CPUGraphWidget(const FontManager* fontManager, const IRetroGraph& rg, bool visible);

    void updateObservers(const IRetroGraph& rg) override { m_cpuMeasure = &rg.getCPUMeasure(); };
    void draw() const override;
private:
    const CPUMeasure* m_cpuMeasure{ nullptr };

    VBOID m_cpuVBO;
};

export class RAMGraphWidget : public Widget {
public:
    RAMGraphWidget(const FontManager* fontManager, const IRetroGraph& rg, bool visible);

    void updateObservers(const IRetroGraph& rg) override { m_ramMeasure = &rg.getRAMMeasure(); };
    void draw() const override;
private:
    const RAMMeasure* m_ramMeasure{ nullptr };

    VBOID m_ramVBO;
};

export class NetGraphWidget : public Widget {
public:
    NetGraphWidget(const FontManager* fontManager, const IRetroGraph& rg, bool visible);

    void updateObservers(const IRetroGraph& rg) override { m_netMeasure = &rg.getNetMeasure(); };
    void draw() const override;
private:
    const NetMeasure* m_netMeasure{ nullptr };

    VBOID m_netUpVBO;
    VBOID m_netDownVBO;
};

}
