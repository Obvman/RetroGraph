#pragma once

#include "stdafx.h"

#include <cstdint>
#include <memory>
#include <map>
#include <string>

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
#include "FPSLimiter.h"

namespace rg {

class RetroGraph {
public:
    RetroGraph(HINSTANCE hInstance);
    ~RetroGraph() = default;
    RetroGraph(const RetroGraph&) = delete;
    RetroGraph& operator=(const RetroGraph&) = delete;
    RetroGraph(RetroGraph&&) = delete;
    RetroGraph& operator=(RetroGraph&&) = delete;

    void update(uint32_t ticks);
    void draw(uint32_t ticks) const;

    void updateWindowSize(int32_t width, int32_t height);

    void needsRedraw() const { m_renderer.needsRedraw(); }

    const std::unique_ptr<CPUMeasure>& getCPUMeasure() const { return m_cpuMeasure; }
    const std::unique_ptr<GPUMeasure>& getGPUMeasure() const { return m_gpuMeasure; }
    const std::unique_ptr<RAMMeasure>& getRAMMeasure() const { return m_ramMeasure; }
    const std::unique_ptr<NetMeasure>& getNetMeasure() const { return m_netMeasure; }
    const std::unique_ptr<ProcessMeasure>& getProcessMeasure() const { return m_processMeasure; }
    const std::unique_ptr<DriveMeasure>& getDriveMeasure() const { return m_driveMeasure; }
    const std::unique_ptr<MusicMeasure>& getMusicMeasure() const { return m_musicMeasure; }
    const std::unique_ptr<SystemMeasure>& getSystemMeasure() const { return m_systemMeasure; }
    const std::unique_ptr<AnimationState>& getAnimationState() const { return m_animationState; }

    bool isRunning() const { return m_window.isRunning(); }

    void toggleWidget(WidgetType w);

private:
    void checkDependencies(); // Disables any measures that aren't currently used TODO rename this

    Window m_window;

    // Measures are shared among widgets so we need these so we know to disable
    // a measure only when there are no widgets using it.
    std::vector<bool> m_widgetVisibilities;

    /* Measure data acquisition/updating is managed by the lifetime of the
     * object, so wrapping them in smart pointers let's us disable/enable
     * measures by destroying/creating the objects
     */
    std::unique_ptr<CPUMeasure> m_cpuMeasure; // done
    std::unique_ptr<GPUMeasure> m_gpuMeasure; // done
    std::unique_ptr<RAMMeasure> m_ramMeasure; // done
    std::unique_ptr<NetMeasure> m_netMeasure; // done
    std::unique_ptr<ProcessMeasure> m_processMeasure; // done
    std::unique_ptr<DriveMeasure> m_driveMeasure; // done
    std::unique_ptr<MusicMeasure> m_musicMeasure; // done
    std::unique_ptr<SystemMeasure> m_systemMeasure; // done
    std::unique_ptr<AnimationState> m_animationState; // done

    Renderer m_renderer;

    // Specifies which widgets rely on which measures.
    // IMPORTANT: Must be updated everytime we modify widgets or their observer pointers to measures!
    // TODO figure out type other than string for measures
    std::map<std::string, std::vector<WidgetType>> m_dependencyMap;
};

} // namespace rg
