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
    GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<GPUMeasure const> gpuMeasure, bool visible);

    void draw() const override;
private:
    std::shared_ptr<GPUMeasure const> m_gpuMeasure{ nullptr };

    VBOID m_gpuVBO;
};

export class CPUGraphWidget : public Widget {
public:
    CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<CPUMeasure const> cpuMeasure, bool visible);

    void draw() const override;
private:
    std::shared_ptr<CPUMeasure const> m_cpuMeasure{ nullptr };

    VBOID m_cpuVBO;
};

export class RAMGraphWidget : public Widget {
public:
    RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<RAMMeasure const> ramMeasure, bool visible);

    void draw() const override;
private:
    std::shared_ptr<RAMMeasure const> m_ramMeasure{ nullptr };

    VBOID m_ramVBO;
};

export class NetGraphWidget : public Widget {
public:
    NetGraphWidget(const FontManager* fontManager, std::shared_ptr<NetMeasure const> netMeasure, bool visible);

    void draw() const override;
private:
    std::shared_ptr<NetMeasure const> m_netMeasure{ nullptr };

    VBOID m_netUpVBO;
    VBOID m_netDownVBO;
};

}
