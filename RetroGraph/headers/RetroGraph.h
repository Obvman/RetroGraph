#pragma once

#include <stdint.h>

#include "Window.h"

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "NetMeasure.h"
#include "DriveMeasure.h"
#include "MusicMeasure.h"

#include "Renderer.h"

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

    const CPUMeasure& getCPUMeasure() const { return m_cpuMeasure; }
    const GPUMeasure& getGPUMeasure() const { return m_gpuMeasure; }
    const RAMMeasure& getRAMMeasure() const { return m_ramMeasure; }
    const NetMeasure& getNetMeasure() const { return m_netMeasure; }
    const ProcessMeasure& getProcessMeasure() const { return m_processMeasure; }
    const DriveMeasure& getDriveMeasure() const { return m_driveMeasure; }
    const MusicMeasure& getMusicMeasure() const { return m_musicMeasure; }
    const SystemMeasure& getSystemMeasure() const { return m_systemMeasure; }
    const UserSettings& getUserSettings() const { return m_userSettings; }

    bool isRunning() const { return m_window.isRunning(); }
private:
    UserSettings m_userSettings;

    Window m_window;

    CPUMeasure m_cpuMeasure;
    GPUMeasure m_gpuMeasure;
    RAMMeasure m_ramMeasure;
    NetMeasure m_netMeasure;
    ProcessMeasure m_processMeasure;
    DriveMeasure m_driveMeasure;
    MusicMeasure m_musicMeasure;
    SystemMeasure m_systemMeasure;

    Renderer m_renderer;
};

}
