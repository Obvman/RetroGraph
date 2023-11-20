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

    void update(int ticks) override;
    void draw(int ticks) const override;

    void updateWindowSize(int width, int height) override;
    void toggleWidget(WidgetType w) override;
    bool isRunning() const override { return m_window.isRunning(); }
    void shutdown() override;

private:
    template<std::derived_from<Measure> T>
    std::shared_ptr<T const> getMeasure() const {
        MeasureType measureType;
        if constexpr (std::is_same_v<T, CPUMeasure>) {
            measureType = MeasureType::CPU;
        } else if constexpr (std::is_same_v<T, GPUMeasure>) {
            measureType = MeasureType::GPU;
        } else if constexpr (std::is_same_v<T, RAMMeasure>) {
            measureType = MeasureType::RAM;
        } else if constexpr (std::is_same_v<T, NetMeasure>) {
            measureType = MeasureType::Net;
        } else if constexpr (std::is_same_v<T, ProcessMeasure>) {
            measureType = MeasureType::Process;
        } else if constexpr (std::is_same_v<T, DriveMeasure>) {
            measureType = MeasureType::Drive;
        } else if constexpr (std::is_same_v<T, MusicMeasure>) {
            measureType = MeasureType::Music;
        } else if constexpr (std::is_same_v<T, SystemMeasure>) {
            measureType = MeasureType::System;
        } else if constexpr (std::is_same_v<T, AnimationState>) {
            measureType = MeasureType::AnimationState;
        } else if constexpr (std::is_same_v<T, DisplayMeasure>) {
            measureType = MeasureType::Display;
        } else {
            static_assert (always_false<T>, "Unknown measure type.");
        }

        auto& measure{ m_measures[static_cast<int>(measureType)] };
        if (!measure)
            measure = createMeasure(measureType);
        return dynamic_pointer_cast<T const> (measure);
    }

    void draw(int ticks, HWND window, HGLRC hrc, int totalFPS) const;
    void setWidgetVisibility(WidgetType w, bool v);
    void setViewports(int windowWidth, int windowHeight);
    std::unique_ptr<Widget> createWidget(WidgetType widgetType) const;
    void destroyWidget(WidgetType widgetType);
    auto createWidgets() const;
    auto createWidgetContainers() const;

    void updateWidgetVisibilities();
    void refreshConfig(int ticks);

    void cleanupUnusedMeasures();
    std::shared_ptr<Measure> createMeasure(MeasureType measureType) const;

    // Measures are shared among widgets so we need these so we know to disable
    // a measure only when there are no widgets using it.
    std::vector<bool> m_widgetVisibilities;

    /* Measure data acquisition/updating is managed by the lifetime of the
     * object, so wrapping them in smart pointers lets us disable/enable
     * measures by destroying/creating the objects
     */
    mutable std::vector<std::shared_ptr<Measure>> m_measures; // #TODO mutable

    Window m_window;

    HWND m_renderTargetHandle{ nullptr };
    FontManager m_fontManager;
    std::vector<std::unique_ptr<Widget>> m_widgets;
    std::vector<std::unique_ptr<WidgetContainer>> m_widgetContainers;
};

} // namespace rg
