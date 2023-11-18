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
    std::map<MeasureType, std::vector<WidgetType>> m_dependencyMap;
};

auto RetroGraph::createMeasures() const {
    decltype(m_measures) measureList( static_cast<int>(MeasureType::NumMeasures) );

    measureList[static_cast<int>(MeasureType::CPU)] =               std::make_unique<CPUMeasure>();
    measureList[static_cast<int>(MeasureType::GPU)] =               std::make_unique<GPUMeasure>();
    measureList[static_cast<int>(MeasureType::RAM)] =               std::make_unique<RAMMeasure>();
    measureList[static_cast<int>(MeasureType::Net)] =               std::make_unique<NetMeasure>();
    measureList[static_cast<int>(MeasureType::Process)] =           std::make_unique<ProcessMeasure>();
    measureList[static_cast<int>(MeasureType::Drive)] =             std::make_unique<DriveMeasure>();
    measureList[static_cast<int>(MeasureType::Music)] =             std::make_unique<MusicMeasure>(dynamic_cast<const ProcessMeasure&>(*measureList[static_cast<int>(MeasureType::Process)]));
    measureList[static_cast<int>(MeasureType::System)] =            std::make_unique<SystemMeasure>();
    measureList[static_cast<int>(MeasureType::AnimationState)] =    std::make_unique<AnimationState>();
    measureList[static_cast<int>(MeasureType::Display)] =           std::make_unique<DisplayMeasure>();

    return measureList;
}

RetroGraph::RetroGraph(HINSTANCE hInstance)
    : m_measures( createMeasures() )
    , m_window{ this, hInstance, UserSettings::inst().getVal<int>("Window.Monitor") }
    , m_widgetVisibilities( static_cast<int>(WidgetType::NumWidgets) )
    , m_renderer{ std::make_unique<Renderer>(m_window.getHwnd(), m_window.getWidth(), m_window.getHeight(), *this)}
    , m_dependencyMap{
        { MeasureType::AnimationState, { WidgetType::Main } },
        { MeasureType::Music,   { WidgetType::Music } },
        { MeasureType::Process, { WidgetType::Music, WidgetType::ProcessCPU, WidgetType::ProcessRAM } },
        { MeasureType::System,  { WidgetType::SystemStats } },
        { MeasureType::Net,     { WidgetType::Time, WidgetType::NetStats, WidgetType::NetGraph } },
        { MeasureType::CPU,     { WidgetType::CPUStats, WidgetType::CPUGraph, WidgetType::SystemStats, WidgetType::Time } },
        { MeasureType::GPU,     { WidgetType::GPUGraph} },
        { MeasureType::RAM,     { WidgetType::RAMGraph } },
        { MeasureType::Drive,   { WidgetType::HDD } },
        { MeasureType::Display, {} }, // Does have dependent widgets, but must not be destroyed
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
    for (auto i = size_t{ 0U }; i < static_cast<int>(MeasureType::NumMeasures); ++i) {
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
        (m_widgetVisibilities[static_cast<int>(WidgetType::Main)]) ? getAnimationState().getAnimationFPS() : 2
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
            const auto w{ static_cast<WidgetType>(i) };
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

void RetroGraph::toggleWidget(WidgetType w) {
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
    return dynamic_cast<const CPUMeasure&>(*m_measures[static_cast<int>(MeasureType::CPU)]); 
}

const GPUMeasure& RetroGraph::getGPUMeasure() const { 
    return dynamic_cast<const GPUMeasure&>(*m_measures[static_cast<int>(MeasureType::GPU)]);
}

const RAMMeasure& RetroGraph::getRAMMeasure() const { 
    return dynamic_cast<const RAMMeasure&>(*m_measures[static_cast<int>(MeasureType::RAM)]);
}

const NetMeasure& RetroGraph::getNetMeasure() const { 
    return dynamic_cast<const NetMeasure&>(*m_measures[static_cast<int>(MeasureType::Net)]);
}

const ProcessMeasure& RetroGraph::getProcessMeasure() const { 
    return dynamic_cast<const ProcessMeasure&>(*m_measures[static_cast<int>(MeasureType::Process)]);
}

const DriveMeasure& RetroGraph::getDriveMeasure() const { 
    return dynamic_cast<const DriveMeasure&>(*m_measures[static_cast<int>(MeasureType::Drive)]);
}

const MusicMeasure& RetroGraph::getMusicMeasure() const { 
    return dynamic_cast<const MusicMeasure&>(*m_measures[static_cast<int>(MeasureType::Music)]);
}

const SystemMeasure& RetroGraph::getSystemMeasure() const { 
    return dynamic_cast<const SystemMeasure&>(*m_measures[static_cast<int>(MeasureType::System)]);
}

const AnimationState& RetroGraph::getAnimationState() const { 
    return dynamic_cast<const AnimationState&>(*m_measures[static_cast<int>(MeasureType::AnimationState)]);
}

const DisplayMeasure & RetroGraph::getDisplayMeasure() const {
    return dynamic_cast<const DisplayMeasure&>(*m_measures[static_cast<int>(MeasureType::Display)]);
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
                case MeasureType::AnimationState:
                    measurePtr = std::make_unique<AnimationState>();
                    break;
                case MeasureType::Drive:
                    measurePtr = std::make_unique<DriveMeasure>();
                    break;
                case MeasureType::Music:
                    measurePtr = std::make_unique<MusicMeasure>(getProcessMeasure());
                    break;
                case MeasureType::Net:
                    measurePtr = std::make_unique<NetMeasure>();
                    break;
                case MeasureType::System:
                    measurePtr = std::make_unique<SystemMeasure>();
                    break;
                case MeasureType::Process:
                    measurePtr = std::make_unique<ProcessMeasure>();
                    break;
                case MeasureType::RAM:
                    measurePtr = std::make_unique<RAMMeasure>();
                    break;
                case MeasureType::CPU:
                    measurePtr = std::make_unique<CPUMeasure>();
                    break;
                case MeasureType::GPU:
                    measurePtr = std::make_unique<GPUMeasure>();
                    break;
                case MeasureType::Display:
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
    for (auto i = size_t{ 0U }; i < static_cast<int>(WidgetType::NumWidgets); ++i)
        m_widgetVisibilities[i] = UserSettings::inst().isVisible(static_cast<WidgetType>(i));
}

}
