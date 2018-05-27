#include "stdafx.h"

#include "RetroGraph.h"

#include <iostream>

#include "drawUtils.h"
#include "Renderer.h"

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "NetMeasure.h"
#include "ProcessMeasure.h"
#include "DriveMeasure.h"
#include "MusicMeasure.h"
#include "SystemMeasure.h"
#include "AnimationState.h"

namespace rg {

using MTypes = Measures::Types;

RetroGraph::RetroGraph(HINSTANCE hInstance) :
    m_window{ this, hInstance, 
              std::get<int32_t>(UserSettings::inst().getVal("Window.Monitor")), 
              std::get<bool>(UserSettings::inst().getVal("Window.ClickThrough")) },
    m_widgetVisibilities( Widgets::NumWidgets ),
    m_measures( createMeasures() ),
    m_renderer{ std::make_unique<Renderer>(m_window, *this) },
    m_dependencyMap{
        { MTypes::AnimationState, { Widgets::Main } },
        { MTypes::Music,   { Widgets::Music } },
        { MTypes::Process, { Widgets::Music, Widgets::ProcessCPU, Widgets::ProcessRAM } },
        { MTypes::System,  { Widgets::SystemStats } },
        { MTypes::Net,     { Widgets::Graph, Widgets::Time, Widgets::SystemStats } },
        { MTypes::CPU,     { Widgets::CPUStats, Widgets::Graph, Widgets::SystemStats, Widgets::Time } },
        { MTypes::GPU,     { Widgets::Graph } },
        { MTypes::RAM,     { Widgets::Graph } },
        { MTypes::Drive,   { Widgets::HDD } },
    } 
    {

    for (auto i = size_t{ 0U }; i < Widgets::NumWidgets; ++i) {
        m_widgetVisibilities[i] = UserSettings::inst().isVisible(static_cast<Widgets>(i));
    }

    checkDependencies();

    update(0);
    draw(0);
}

RetroGraph::~RetroGraph() {
}

auto RetroGraph::createMeasures() -> decltype(m_measures) {
    decltype(m_measures) measureList( Measures::Types::NumMeasures );

    measureList[MTypes::CPU] = std::make_unique<CPUMeasure>();
    measureList[MTypes::GPU] = std::make_unique<GPUMeasure>();
    measureList[MTypes::RAM] = std::make_unique<RAMMeasure>();
    measureList[MTypes::Net] = std::make_unique<NetMeasure>();
    measureList[MTypes::Process] = std::make_unique<ProcessMeasure>();
    measureList[MTypes::Drive] = std::make_unique<DriveMeasure>();
    measureList[MTypes::Music] = std::make_unique<MusicMeasure>(
        dynamic_cast<const ProcessMeasure&>(*measureList[MTypes::Process]));
    measureList[MTypes::System] = std::make_unique<SystemMeasure>();
    measureList[MTypes::AnimationState] = std::make_unique<AnimationState>();

    return measureList;
}

void RetroGraph::update(uint32_t ticks) {
    // Update with a tick offset so all measures don't update in the same
    // cycle and spike the CPU
    auto offset = uint32_t{ 0U };
    for (auto i = size_t{ 0U }; i < MTypes::NumMeasures; ++i) {
        const auto& measurePtr{ m_measures[i] };
        if (measurePtr) {
            measurePtr->update(ticks + ++offset);
        }
    }
}

void RetroGraph::draw(uint32_t ticks) const {
    // If the main widget is running, draw at it's target FPS,
    // Otherwise, we don't have to waste cycles swapping buffers when
    // the other measures update twice a second, so just draw at 2 FPS
    const auto framesPerSecond = uint32_t{ (m_widgetVisibilities[Widgets::Main]) ? 
       getAnimationState().getAnimationFPS() : 2U };

    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

        HDC hdc = GetDC(m_window.getHwnd());
        wglMakeCurrent(hdc, m_window.getHGLRC());

        m_renderer->draw(ticks);

        SwapBuffers(hdc);
        ReleaseDC(m_window.getHwnd(), hdc);

        FPSLimiter::inst().end();
        FPSLimiter::inst().begin();
    }
}

void RetroGraph::updateWindowSize(int32_t width, int32_t height) {
    m_renderer->updateWindowSize(width, height);
}

void RetroGraph::needsRedraw() const {
     m_renderer->needsRedraw();
}

void RetroGraph::toggleWidget(Widgets w) {
    m_widgetVisibilities[w] = !m_widgetVisibilities[w];
    checkDependencies();
    m_renderer->setWidgetVisibility(w, m_widgetVisibilities[w]);
}

void RetroGraph::checkDependencies() {
    // Check dependent measures, if theres a measure that isn't being used by any
    // Widget, we can disable it. If a disabled measure needs to be used by a widget,
    // re-enable it
    for (const auto& [measure, widgets] : m_dependencyMap) {

        bool allDependentWidgetsDisabled{ true };
        for (const auto& w : widgets) {
            if (m_widgetVisibilities[w]) {
                allDependentWidgetsDisabled = false;
                break;
            }
        }

        // Enable/Disable measures depending on whether all it's dependencies
        // have toggled
        auto& measurePtr{ m_measures[measure] };
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
                default: // nothing
                    break;
            }
        }
    }

    // Most Widgets store observers to Measures, this updates their value
    // in case of destruction/construction of Measures
    m_renderer->updateObservers(*this);
}

const CPUMeasure& RetroGraph::getCPUMeasure() const {
    return dynamic_cast<const CPUMeasure&>(*m_measures[MTypes::CPU]); 
}
const GPUMeasure& RetroGraph::getGPUMeasure() const { 
    return dynamic_cast<const GPUMeasure&>(*m_measures[MTypes::GPU]);
}
const RAMMeasure& RetroGraph::getRAMMeasure() const { 
    return dynamic_cast<const RAMMeasure&>(*m_measures[MTypes::RAM]);
}
const NetMeasure& RetroGraph::getNetMeasure() const { 
    return dynamic_cast<const NetMeasure&>(*m_measures[MTypes::Net]);
}
const ProcessMeasure& RetroGraph::getProcessMeasure() const { 
    return dynamic_cast<const ProcessMeasure&>(*m_measures[MTypes::Process]);
}
const DriveMeasure& RetroGraph::getDriveMeasure() const { 
    return dynamic_cast<const DriveMeasure&>(*m_measures[MTypes::Drive]);
}
const MusicMeasure& RetroGraph::getMusicMeasure() const { 
    return dynamic_cast<const MusicMeasure&>(*m_measures[MTypes::Music]);
}
const SystemMeasure& RetroGraph::getSystemMeasure() const { 
    return dynamic_cast<const SystemMeasure&>(*m_measures[MTypes::System]);
}
const AnimationState& RetroGraph::getAnimationState() const { 
    return dynamic_cast<const AnimationState&>(*m_measures[MTypes::AnimationState]);
}


}
