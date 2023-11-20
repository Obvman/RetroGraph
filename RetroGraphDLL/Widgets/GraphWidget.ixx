export module Widgets.GraphWidget;

import Measures.CPUMeasure;
import Measures.GPUMeasure;
import Measures.NetMeasure;
import Measures.RAMMeasure;

import Rendering.FontManager;
import Rendering.VBO;

import Widgets.Widget;

import std.memory;

namespace rg {

export class GPUGraphWidget : public Widget {
public:
    GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure);

    void draw() const override;
private:
    std::shared_ptr<const GPUMeasure> m_gpuMeasure{ nullptr };

    VBOID m_gpuVBO;
};

export class CPUGraphWidget : public Widget {
public:
    CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure);

    void draw() const override;
private:
    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };

    VBOID m_cpuVBO;
};

export class RAMGraphWidget : public Widget {
public:
    RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure);

    void draw() const override;
private:
    std::shared_ptr<const RAMMeasure> m_ramMeasure{ nullptr };

    VBOID m_ramVBO;
};

export class NetGraphWidget : public Widget {
public:
    NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure);

    void draw() const override;
private:
    std::shared_ptr<const NetMeasure> m_netMeasure{ nullptr };

    VBOID m_netUpVBO;
    VBOID m_netDownVBO;
};

}
