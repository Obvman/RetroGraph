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
    ~GPUGraphWidget();

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const GPUMeasure> m_gpuMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
};

export class CPUGraphWidget : public GraphWidget {
public:
    CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure);
    ~CPUGraphWidget();

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const CPUMeasure> m_cpuMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
};

export class RAMGraphWidget : public GraphWidget {
public:
    RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure);
    ~RAMGraphWidget();

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const RAMMeasure> m_ramMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
};

export class NetGraphWidget : public GraphWidget {
public:
    NetGraphWidget(const FontManager* fontManager, std::shared_ptr<const NetMeasure> netMeasure);
    ~NetGraphWidget();

    void draw() const override;

private:
    PostUpdateCallbackHandle RegisterPostUpdateCallback();

    std::shared_ptr<const NetMeasure> m_netMeasure{ nullptr };
    PostUpdateCallbackHandle m_postUpdateHandle;
    LineGraph m_netUpGraph;
};

}
