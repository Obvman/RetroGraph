export module IRetroGraph;

import Measures.AnimationState;
import Measures.CPUMeasure;
import Measures.DisplayMeasure;
import Measures.DriveMeasure;
import Measures.GPUMeasure;
import Measures.NetMeasure;
import Measures.RAMMeasure;
import Measures.MusicMeasure;
import Measures.ProcessMeasure;
import Measures.SystemMeasure;

import Widgets.WidgetDefines;

namespace rg {

export class IRetroGraph {
public:
    virtual ~IRetroGraph() = default;

    virtual const AnimationState& getAnimationState() const = 0;
    virtual const CPUMeasure& getCPUMeasure() const = 0;
    virtual const DisplayMeasure& getDisplayMeasure() const = 0;
    virtual const DriveMeasure& getDriveMeasure() const = 0;
    virtual const GPUMeasure& getGPUMeasure() const = 0;
    virtual const MusicMeasure& getMusicMeasure() const = 0;
    virtual const NetMeasure& getNetMeasure() const = 0;
    virtual const ProcessMeasure& getProcessMeasure() const = 0;
    virtual const RAMMeasure& getRAMMeasure() const = 0;
    virtual const SystemMeasure& getSystemMeasure() const = 0;

    virtual void update(int ticks) = 0;
    virtual void draw(int ticks) const = 0;
    virtual bool isRunning() const = 0;
    virtual void toggleWidget(WidgetType w) = 0;
    virtual void updateWindowSize(int width, int height) = 0;
    virtual void shutdown() = 0;
};

}
