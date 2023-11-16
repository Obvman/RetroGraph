module;

// Error if this is not here, even though it's included in WindowsHeaders.h
#include <sys/stat.h>

export module RetroGraph;

import FPSLimiter;
import IRetroGraph;
import UserSettings;
import Window;
import Utils;

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

import Rendering.DrawUtils;
import Rendering.Renderer;

import Widgets.WidgetDefines;

import std.core;

import "WindowsHeaders.h";

namespace rg {

export class RetroGraph;

class RetroGraph : public IRetroGraph {
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
    void toggleWidget(Widgets w) override;
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
    void checkConfigChanged(int ticks);
    void cleanupUnusedMeasures();
    void updateWidgetVisibilities();
    void refreshConfig(bool autoReadConfig);

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
    std::map<Measures::Types, std::vector<Widgets>> m_dependencyMap;
};

using MTypes = Measures::Types;

auto RetroGraph::createMeasures() const {
    decltype(m_measures) measureList( static_cast<int>(Measures::Types::NumMeasures) );

    measureList[static_cast<int>(MTypes::CPU)] =               std::make_unique<CPUMeasure>();
    measureList[static_cast<int>(MTypes::GPU)] =               std::make_unique<GPUMeasure>();
    measureList[static_cast<int>(MTypes::RAM)] =               std::make_unique<RAMMeasure>();
    measureList[static_cast<int>(MTypes::Net)] =               std::make_unique<NetMeasure>();
    measureList[static_cast<int>(MTypes::Process)] =           std::make_unique<ProcessMeasure>();
    measureList[static_cast<int>(MTypes::Drive)] =             std::make_unique<DriveMeasure>();
    measureList[static_cast<int>(MTypes::Music)] =             std::make_unique<MusicMeasure>(dynamic_cast<const ProcessMeasure&>(*measureList[static_cast<int>(MTypes::Process)]));
    measureList[static_cast<int>(MTypes::System)] =            std::make_unique<SystemMeasure>();
    measureList[static_cast<int>(MTypes::AnimationState)] =    std::make_unique<AnimationState>();
    measureList[static_cast<int>(MTypes::Display)] =           std::make_unique<DisplayMeasure>();

    return measureList;
}

RetroGraph::RetroGraph(HINSTANCE hInstance)
    : m_measures( createMeasures() )
    , m_window{ this, hInstance, UserSettings::inst().getVal<int>("Window.Monitor") }
    , m_widgetVisibilities( static_cast<int>(Widgets::NumWidgets) )
    , m_renderer{ std::make_unique<Renderer>(m_window.getHwnd(), m_window.getWidth(), m_window.getHeight(), *this)}
    , m_dependencyMap{
        { MTypes::AnimationState, { Widgets::Main } },
        { MTypes::Music,   { Widgets::Music } },
        { MTypes::Process, { Widgets::Music, Widgets::ProcessCPU, Widgets::ProcessRAM } },
        { MTypes::System,  { Widgets::SystemStats } },
        { MTypes::Net,     { Widgets::Time, Widgets::NetStats, Widgets::NetGraph } },
        { MTypes::CPU,     { Widgets::CPUStats, Widgets::CPUGraph, Widgets::SystemStats, Widgets::Time } },
        { MTypes::GPU,     { Widgets::GPUGraph} },
        { MTypes::RAM,     { Widgets::RAMGraph } },
        { MTypes::Drive,   { Widgets::HDD } },
        { MTypes::Display, {} }, // Does have dependent widgets, but must not be destroyed
    } {

    updateWidgetVisibilities();

    cleanupUnusedMeasures();

    update(0);
    draw(0);
}

void RetroGraph::update(int ticks) {
    checkConfigChanged(ticks);

    // Update with a tick offset so all measures don't update in the same
    // cycle and spike the CPU
    auto offset = int{ 0U };
    for (auto i = size_t{ 0U }; i < static_cast<int>(MTypes::NumMeasures); ++i) {
        const auto& measurePtr{ m_measures[i] };
        if (measurePtr && measurePtr->shouldUpdate(ticks + ++offset)) {
            measurePtr->update(ticks + offset);
        }
    }
}

void RetroGraph::draw(int ticks) const {
    // If the main widget is running, draw at it's target FPS,
    // Otherwise, we don't have to waste cycles swapping buffers when
    // the other measures update twice a second, so just draw at 2 FPS
    const auto framesPerSecond = int{ 
        (m_widgetVisibilities[static_cast<int>(Widgets::Main)]) ? getAnimationState().getAnimationFPS() : 2
    };

    m_renderer->draw(ticks, m_window.getHwnd(), m_window.getHGLRC(), framesPerSecond);
}

void RetroGraph::checkConfigChanged(int ticks) {
    static const auto autoReadConfig{ UserSettings::inst().getVal<bool>("Application.AutoReadConfig") };

    if (ticksMatchSeconds(ticks, 5) && autoReadConfig ) {
        struct stat result;
        if (stat(UserSettings::iniPath.c_str(), &result) == 0) {
            auto newModTime{ result.st_mtime };
            static auto modTime{ newModTime };

            if (newModTime != modTime)
                refreshConfig(autoReadConfig);

            modTime = newModTime;
        }
    }
}

void RetroGraph::refreshConfig(bool autoReadConfig) {
    UserSettings::inst().readConfig();

    autoReadConfig = UserSettings::inst().getVal<bool>("Application.AutoReadConfig");
    if (!autoReadConfig)
        return;

    const auto oldWidgetVisibilites{ m_widgetVisibilities };
    updateWidgetVisibilities();
    for (auto i = size_t{ 0U }; i < m_widgetVisibilities.size(); ++i) {
        if (oldWidgetVisibilites[i] != m_widgetVisibilities[i]) {
            cleanupUnusedMeasures();
            const auto w{ static_cast<Widgets>(i) };
            m_renderer->setWidgetVisibility(w, m_widgetVisibilities[static_cast<int>(w)]);
        }
    }

    for (const auto& pm : m_measures)
        if (pm)
            pm->refreshSettings();

    m_window.refreshSettings();
    m_renderer->setViewports(m_window.getWidth(), m_window.getHeight());

    draw(0);
}

void RetroGraph::updateWindowSize(int width, int height) {
    m_renderer->updateWindowSize(width, height);
}

void RetroGraph::toggleWidget(Widgets w) {
    m_widgetVisibilities[static_cast<int>(w)] = !m_widgetVisibilities[static_cast<int>(w)];
    cleanupUnusedMeasures();

    m_renderer->setWidgetVisibility(w, m_widgetVisibilities[static_cast<int>(w)]);
    m_renderer->setViewports(m_window.getWidth(), m_window.getHeight());
    m_renderer->draw(0, m_window.getHwnd(), m_window.getHGLRC(), 1);
}

void RetroGraph::shutdown() {
    UserSettings::inst().writeDataFile();
}

const CPUMeasure& RetroGraph::getCPUMeasure() const {
    return dynamic_cast<const CPUMeasure&>(*m_measures[static_cast<int>(MTypes::CPU)]); 
}

const GPUMeasure& RetroGraph::getGPUMeasure() const { 
    return dynamic_cast<const GPUMeasure&>(*m_measures[static_cast<int>(MTypes::GPU)]);
}

const RAMMeasure& RetroGraph::getRAMMeasure() const { 
    return dynamic_cast<const RAMMeasure&>(*m_measures[static_cast<int>(MTypes::RAM)]);
}

const NetMeasure& RetroGraph::getNetMeasure() const { 
    return dynamic_cast<const NetMeasure&>(*m_measures[static_cast<int>(MTypes::Net)]);
}

const ProcessMeasure& RetroGraph::getProcessMeasure() const { 
    return dynamic_cast<const ProcessMeasure&>(*m_measures[static_cast<int>(MTypes::Process)]);
}

const DriveMeasure& RetroGraph::getDriveMeasure() const { 
    return dynamic_cast<const DriveMeasure&>(*m_measures[static_cast<int>(MTypes::Drive)]);
}

const MusicMeasure& RetroGraph::getMusicMeasure() const { 
    return dynamic_cast<const MusicMeasure&>(*m_measures[static_cast<int>(MTypes::Music)]);
}

const SystemMeasure& RetroGraph::getSystemMeasure() const { 
    return dynamic_cast<const SystemMeasure&>(*m_measures[static_cast<int>(MTypes::System)]);
}

const AnimationState& RetroGraph::getAnimationState() const { 
    return dynamic_cast<const AnimationState&>(*m_measures[static_cast<int>(MTypes::AnimationState)]);
}

const DisplayMeasure & RetroGraph::getDisplayMeasure() const {
    return dynamic_cast<const DisplayMeasure&>(*m_measures[static_cast<int>(MTypes::Display)]);
}

void RetroGraph::cleanupUnusedMeasures() {
    // Check dependent measures, if theres a measure that isn't being used by any
    // Widget, we can disable it. If a disabled measure needs to be used by a widget,
    // re-enable it
    for (const auto& [measure, widgets] : m_dependencyMap) {
        // Measures with no widget dependencies will always exist.
        if (widgets.empty()) 
            continue;

        bool allDependentWidgetsDisabled{ true };
        for (const auto& w : widgets) {
            if (m_widgetVisibilities[static_cast<int>(w)]) {
                allDependentWidgetsDisabled = false;
                break;
            }
        }

        // Enable/Disable measures depending on whether all it's dependencies
        // have toggled
        auto& measurePtr{ m_measures[static_cast<int>(measure)] };
        if (allDependentWidgetsDisabled) {
            if (measurePtr) {
                measurePtr.reset(nullptr);
            }
        } else if (!measurePtr) {
            switch (measure) {
                case MTypes::AnimationState:
                    measurePtr = std::make_unique<AnimationState>();
                    break;
                case MTypes::Drive:
                    measurePtr = std::make_unique<DriveMeasure>();
                    break;
                case MTypes::Music:
                    measurePtr = std::make_unique<MusicMeasure>(getProcessMeasure());
                    break;
                case MTypes::Net:
                    measurePtr = std::make_unique<NetMeasure>();
                    break;
                case MTypes::System:
                    measurePtr = std::make_unique<SystemMeasure>();
                    break;
                case MTypes::Process:
                    measurePtr = std::make_unique<ProcessMeasure>();
                    break;
                case MTypes::RAM:
                    measurePtr = std::make_unique<RAMMeasure>();
                    break;
                case MTypes::CPU:
                    measurePtr = std::make_unique<CPUMeasure>();
                    break;
                case MTypes::GPU:
                    measurePtr = std::make_unique<GPUMeasure>();
                    break;
                case MTypes::Display:
                    measurePtr = std::make_unique<DisplayMeasure>();
                    break;
                default: // nothing
                    break;
            }
        }
    }

    // Most Widgets store observers to Measures, this updates their value
    // in case of destruction/construction of Measures
    m_renderer->updateObservers(*this);
}

void RetroGraph::updateWidgetVisibilities() {
    for (auto i = size_t{ 0U }; i < static_cast<int>(Widgets::NumWidgets); ++i)
        m_widgetVisibilities[i] = UserSettings::inst().isVisible(static_cast<Widgets>(i));
}

}
