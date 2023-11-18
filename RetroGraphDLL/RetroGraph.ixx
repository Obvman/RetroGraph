export module RetroGraph;

import IRetroGraph;
import Window;

import Measures.AnimationState;
import Measures.CPUMeasure;
import Measures.DisplayMeasure;
import Measures.DriveMeasure;
import Measures.GPUMeasure;
import Measures.Measure;
import Measures.MusicMeasure;
import Measures.NetMeasure;
import Measures.ProcessMeasure;
import Measures.RAMMeasure;
import Measures.SystemMeasure;

import Rendering.Renderer;

import Widgets.WidgetType;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

export class RetroGraph : public IRetroGraph {
public:
    __declspec(dllexport) RetroGraph(HINSTANCE hInstance);
    ~RetroGraph() = default;
    RetroGraph(const RetroGraph&) = delete;
    RetroGraph& operator=(const RetroGraph&) = delete;
    RetroGraph(RetroGraph&&) = delete;
    RetroGraph& operator=(RetroGraph&&) = delete;

    void update(int ticks) override;
    void draw(int ticks) const override;

    void updateWindowSize(int width, int height) override;
    void toggleWidget(WidgetType w) override;
    bool isRunning() const override { return m_window.isRunning(); }
    void shutdown() override;

    const CPUMeasure& getCPUMeasure() const override;
    const GPUMeasure& getGPUMeasure() const override;
    const RAMMeasure& getRAMMeasure() const override;
    const NetMeasure& getNetMeasure() const override;
    const ProcessMeasure& getProcessMeasure() const override;
    const DriveMeasure& getDriveMeasure() const override;
    const MusicMeasure& getMusicMeasure() const override;
    const SystemMeasure& getSystemMeasure() const override;
    const AnimationState& getAnimationState() const override;
    const DisplayMeasure& getDisplayMeasure() const override;

private:
    void cleanupUnusedMeasures();
    void updateWidgetVisibilities();
    void refreshConfig(int ticks);

    // Initialises the measures list
    auto createMeasures() const;

    // Measures are shared among widgets so we need these so we know to disable
    // a measure only when there are no widgets using it.
    std::vector<bool> m_widgetVisibilities;

    /* Measure data acquisition/updating is managed by the lifetime of the
     * object, so wrapping them in smart pointers let's us disable/enable
     * measures by destroying/creating the objects
     */
    std::vector<std::unique_ptr<Measure>> m_measures;

    Window m_window;

    std::unique_ptr<Renderer> m_renderer;

    // Specifies which widgets rely on which measures.
    // IMPORTANT: Must be updated everytime we modify widgets or their observer pointers to measures!
    // #TODO this should be done via shared_ptr instead.
    std::map<MeasureType, std::vector<WidgetType>> m_dependencyMap;
};

} // namespace rg
