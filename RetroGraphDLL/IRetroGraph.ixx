module;

export module IRetroGraph;

import AnimationState;
import CPUMeasure;
import DisplayMeasure;
import DriveMeasure;
import GPUMeasure;
import NetMeasure;
import RAMMeasure;
import MusicMeasure;
import ProcessMeasure;
import SystemMeasure;
import WidgetDefines;

namespace rg {

export class IRetroGraph;

class IRetroGraph {
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
    virtual void toggleWidget(Widgets w) = 0;
    virtual void updateWindowSize(int width, int height) = 0;
    virtual void shutdown() = 0;
};

}
