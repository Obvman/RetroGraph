#pragma once

#include "stdafx.h"

#include <cstdint>
#include <memory>
#include <map>
#include <string>

#include "Window.h"
#include "Measure.h"
#include "UserSettings.h"
#include "FPSLimiter.h"
namespace rg {

class Renderer;
class CPUMeasure;
class GPUMeasure;
class RAMMeasure;
class NetMeasure;
class ProcessMeasure;
class DriveMeasure;
class MusicMeasure;
class SystemMeasure;
class AnimationState;
class SystemInformationMeasure;
class DisplayMeasure;

class RetroGraph {
public:
    RetroGraph(HINSTANCE hInstance);
    ~RetroGraph();
    RetroGraph(const RetroGraph&) = delete;
    RetroGraph& operator=(const RetroGraph&) = delete;
    RetroGraph(RetroGraph&&) = delete;
    RetroGraph& operator=(RetroGraph&&) = delete;

    void update(int ticks);
    void draw(int ticks) const;

    void refreshConfig();
    void updateWindowSize(int width, int height);
    void toggleWidget(Widgets w);
    bool isRunning() const { return m_window.isRunning(); }

    const CPUMeasure& getCPUMeasure() const;
    const GPUMeasure& getGPUMeasure() const;
    const RAMMeasure& getRAMMeasure() const;
    const NetMeasure& getNetMeasure() const;
    const ProcessMeasure& getProcessMeasure() const;
    const DriveMeasure& getDriveMeasure() const;
    const MusicMeasure& getMusicMeasure() const;
    const SystemMeasure& getSystemMeasure() const;
    const SystemInformationMeasure& getSystemInformationMeasure() const;
    const AnimationState& getAnimationState() const;
    const DisplayMeasure& getDisplayMeasure() const;

private:
    void checkConfigChanged(int ticks);
    void checkDependencies(); // Disables any measures that aren't currently used TODO rename this
    void updateWidgetVisibilities();

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

    bool m_autoReadConfig{ true };

    // Specifies which widgets rely on which measures.
    // IMPORTANT: Must be updated everytime we modify widgets or their observer pointers to measures!
    std::map<Measures::Types, std::vector<Widgets>> m_dependencyMap;

    // Initialises the measures list
    auto createMeasures() -> decltype(m_measures);
};

} // namespace rg
