#pragma once

#include <stdint.h>
#include <memory>

#include "Window.h"
#include "Renderer.h"

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "NetMeasure.h"
#include "DriveMeasure.h"
#include "MusicMeasure.h"
#include "SystemMeasure.h"
#include "UserSettings.h"
#include "AnimationState.h"

namespace rg {

class RetroGraph {
public:
    RetroGraph(HINSTANCE hInstance);
    ~RetroGraph() = default;
    RetroGraph(const RetroGraph&) = delete;
    RetroGraph& operator=(const RetroGraph&) = delete;

    void update(uint32_t ticks);
    void draw(uint32_t ticks) const;

    void updateWindowSize(int32_t width, int32_t height);

    void needsRedraw() const { m_renderer.needsRedraw(); }

    const CPUMeasure& getCPUMeasure() const { return m_cpuMeasure; }
    const GPUMeasure& getGPUMeasure() const { return m_gpuMeasure; }
    const RAMMeasure& getRAMMeasure() const { return m_ramMeasure; }
    const NetMeasure& getNetMeasure() const { return m_netMeasure; }
    const ProcessMeasure& getProcessMeasure() const { return m_processMeasure; }
    const std::unique_ptr<DriveMeasure>& getDriveMeasure() const { return m_driveMeasure; }
    const std::unique_ptr<MusicMeasure>& getMusicMeasure() const { return m_musicMeasure; }
    const SystemMeasure& getSystemMeasure() const { return m_systemMeasure; }
    const UserSettings& getUserSettings() const { return m_userSettings; }
    const std::unique_ptr<AnimationState>& getAnimationState() const { return m_animationState; }

    bool isRunning() const { return m_window.isRunning(); }

    void toggleWidget(Widget w);

private:

    const UserSettings& m_userSettings;

    Window m_window;

    /* Measure data acquisition/updating is managed by the lifetime of the
     * object, so wrapping them in smart pointers let's us disable/enable
     * measures by destroying/creating the objects
     */
    CPUMeasure m_cpuMeasure;
    GPUMeasure m_gpuMeasure;
    RAMMeasure m_ramMeasure;
    NetMeasure m_netMeasure;
    ProcessMeasure m_processMeasure;
    std::unique_ptr<DriveMeasure> m_driveMeasure;
    std::unique_ptr<MusicMeasure> m_musicMeasure;
    SystemMeasure m_systemMeasure;
    std::unique_ptr<AnimationState> m_animationState;

    // Measures are shared among widgets so we need these so we know to disable
    // a measure only when there are no widgets using it.
    bool m_timeWidgetEnabled{ true };
    bool m_HDDWidgetEnabled{ true };
    bool m_cpuStatsWidgetEnabled{ true };
    bool m_cpuProcessWidgetEnabled{ true };
    bool m_ramProcessWidgetEnabled{ true };
    bool m_graphWidgetEnabled{ true };
    bool m_mainWidgetEnabled{ true };
    bool m_musicWidgetEnabled{ true };
    bool m_systemStatsWidgetEnabled{ true };

    Renderer m_renderer;
};

}
