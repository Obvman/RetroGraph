export module Widgets.GraphWidget;

import Measures.CPUMeasure;
import Measures.GPUMeasure;
import Measures.NetMeasure;
import Measures.RAMMeasure;

import Rendering.FontManager;

import Widgets.LineGraph;
import Widgets.Widget;

import std.memory;

namespace rg {

class GraphWidget : public Widget {
public:
    GraphWidget(const FontManager* fontManager, size_t numGraphSamples);

protected:
    LineGraph m_graph;
};

export class GPUGraphWidget : public GraphWidget {
public:
    GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure);

    void draw() const override;
private:
    std::shared_ptr<const GPUMeasure> m_gpuMeasure{ nullptr };
};

export class CPUGraphWidget : public GraphWidget {
public:
    CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure);

    void draw() const override;
private:
    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };
};

export class RAMGraphWidget : public GraphWidget {
public:
    RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure);

    void draw() const override;
private:
    std::shared_ptr<const RAMMeasure> m_ramMeasure{ nullptr };
};

export class NetGraphWidget : public GraphWidget {
public:
    NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure);

    void draw() const override;
private:
    std::shared_ptr<const NetMeasure> m_netMeasure{ nullptr };

    LineGraph m_netUpGraph;
};

}
