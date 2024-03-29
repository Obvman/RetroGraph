export module RG.Application:RetroGraph;

import :IRetroGraph;
import :Window;

import FPSCounter;
import FPSLimiter;

import RG.Measures;
import RG.Measures.DataSources;
import RG.Rendering;
import RG.Widgets;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

template<class...>
constexpr std::false_type always_false{};

export class RetroGraph : public IRetroGraph {
public:
    __declspec(dllexport) RetroGraph(HINSTANCE hInstance);
    __declspec(dllexport) ~RetroGraph();
    RetroGraph(const RetroGraph&) = delete;
    RetroGraph& operator=(const RetroGraph&) = delete;
    RetroGraph(RetroGraph&&) = delete;
    RetroGraph& operator=(RetroGraph&&) = delete;

    __declspec(dllexport) void run() override;
    void reloadResources() override;

    void updateWindowSize(int width, int height) override;
    void toggleWidget(WidgetType w) override;
    void toggleWidgetBackgroundVisible() override;
    void shutdown() override;

private:
    void update();
    void draw() const;
    bool isRunning() const { return m_window.isRunning(); }

    void tryRefreshConfig();
    void refreshConfig();

    void getWidgetPropertiesFromSettings();
    void setViewports(int windowWidth, int windowHeight);
    std::unique_ptr<Widget> createWidget(WidgetType widgetType);
    auto createWidgets();
    auto createWidgetContainers() const;
    std::vector<bool> createWidgetVisibilities() const;
    auto createWidgetPositions() const;
    void cleanupUnusedMeasures();

    bool isWidgetVisible(WidgetType w) const { return m_widgets[static_cast<int>(w)] != nullptr; }
    WidgetPosition getWidgetPosition(WidgetType w) const { return m_widgetPositions[static_cast<int>(w)]; }

    ConfigRefreshedEvent::Handle RegisterConfigRefreshedCallback();

    std::shared_ptr<CPUMeasure> m_cpuMeasure;
    std::shared_ptr<GPUMeasure> m_gpuMeasure;
    std::shared_ptr<RAMMeasure> m_ramMeasure;
    std::shared_ptr<NetMeasure> m_netMeasure;
    std::shared_ptr<ProcessMeasure> m_processMeasure;
    std::shared_ptr<DriveMeasure> m_driveMeasure;
    std::shared_ptr<MusicMeasure> m_musicMeasure;
    std::shared_ptr<SystemMeasure> m_systemMeasure;
    std::shared_ptr<AnimationState> m_animationState;
    std::shared_ptr<DisplayMeasure> m_displayMeasure;
    std::shared_ptr<TimeMeasure> m_timeMeasure;

    Window m_window;
    FontManager m_fontManager;
    FPSCounter m_fpsCounter;

    std::vector<WidgetPosition> m_widgetPositions;
    bool m_drawWidgetBackgrounds;
    std::vector<std::unique_ptr<Widget>> m_widgets;
    std::vector<std::unique_ptr<WidgetContainer>> m_widgetContainers;

    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
};

} // namespace rg
