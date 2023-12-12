export module RetroGraph;

import FPSCounter;
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

import Rendering.FontManager;

import Widgets.Widget;
import Widgets.WidgetContainer;
import Widgets.WidgetPosition;
import Widgets.WidgetType;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

template <class...> constexpr std::false_type always_false{};

export class RetroGraph : public IRetroGraph {
public:
    __declspec(dllexport) RetroGraph(HINSTANCE hInstance);
    __declspec(dllexport) ~RetroGraph();
    RetroGraph(const RetroGraph&) = delete;
    RetroGraph& operator=(const RetroGraph&) = delete;
    RetroGraph(RetroGraph&&) = delete;
    RetroGraph& operator=(RetroGraph&&) = delete;

    void update() override;
    void draw() const override;
    void reloadResources() override;

    void updateWindowSize(int width, int height) override;
    void toggleWidget(WidgetType w) override;
    bool isRunning() const override { return m_window.isRunning(); }
    void shutdown() override;
    FPSCounter& getFPSCounter() override { return m_fpsCounter; }

private:
    template<std::derived_from<Measure> T>
    MeasureType getMeasureType() const {
        if constexpr (std::is_same_v<T, CPUMeasure>) {
            return MeasureType::CPU;
        } else if constexpr (std::is_same_v<T, GPUMeasure>) {
            return MeasureType::GPU;
        } else if constexpr (std::is_same_v<T, RAMMeasure>) {
            return MeasureType::RAM;
        } else if constexpr (std::is_same_v<T, NetMeasure>) {
            return MeasureType::Net;
        } else if constexpr (std::is_same_v<T, ProcessMeasure>) {
            return MeasureType::Process;
        } else if constexpr (std::is_same_v<T, DriveMeasure>) {
            return MeasureType::Drive;
        } else if constexpr (std::is_same_v<T, MusicMeasure>) {
            return MeasureType::Music;
        } else if constexpr (std::is_same_v<T, SystemMeasure>) {
            return MeasureType::System;
        } else if constexpr (std::is_same_v<T, AnimationState>) {
            return MeasureType::AnimationState;
        } else if constexpr (std::is_same_v<T, DisplayMeasure>) {
            return MeasureType::Display;
        } else {
            static_assert (always_false<T>, "Unknown measure type.");
        }
    }

    template<std::derived_from<Measure> T>
    std::shared_ptr<const T> getMeasure() {
        MeasureType measureType{ getMeasureType<T>() };
        auto& measure{ m_measures[static_cast<int>(measureType)] };
        if (!measure)
            measure = std::make_shared<T>();
        return dynamic_pointer_cast<const T> (measure);
    }

    void tryRefreshConfig();
    void refreshConfig();
    void setViewports(int windowWidth, int windowHeight);
    std::unique_ptr<Widget> createWidget(WidgetType widgetType);
    auto createWidgets();
    auto createWidgetContainers() const;
    void cleanupUnusedMeasures();

    /* Measure data acquisition/updating is managed by the lifetime of the
     * object, so wrapping them in smart pointers lets us disable/enable
     * measures by destroying/creating the objects
     */
    std::vector<std::shared_ptr<Measure>> m_measures;

    Window m_window;
    FontManager m_fontManager;
    FPSCounter m_fpsCounter;
    std::vector<std::unique_ptr<Widget>> m_widgets;
    std::vector<std::unique_ptr<WidgetContainer>> m_widgetContainers;
};

} // namespace rg
