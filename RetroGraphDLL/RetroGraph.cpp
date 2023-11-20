module RetroGraph;

import Colors;
import FPSLimiter;
import UserSettings;

import Widgets.CPUStatsWidget;
import Widgets.FPSWidget;
import Widgets.GraphWidget;
import Widgets.HDDWidget;
import Widgets.MainWidget;
import Widgets.MusicWidget;
import Widgets.NetStatsWidget;
import Widgets.ProcessCPUWidget;
import Widgets.ProcessRAMWidget;
import Widgets.SystemStatsWidget;
import Widgets.TimeWidget;

import "RGAssert.h";

namespace rg {

std::unique_ptr<Widget> RetroGraph::createWidget(WidgetType widgetType) const {

    const auto& s{ UserSettings::inst() };

    switch (widgetType) {
    case WidgetType::ProcessCPU:
        return std::make_unique<ProcessCPUWidget>(&m_fontManager, getMeasure<ProcessMeasure>(), s.isVisible(WidgetType::ProcessCPU));
    case WidgetType::ProcessRAM:
        return std::make_unique<ProcessRAMWidget>(&m_fontManager, getMeasure<ProcessMeasure>(), s.isVisible(WidgetType::ProcessRAM));
    case WidgetType::Time:
        return std::make_unique<TimeWidget>(&m_fontManager, getMeasure<CPUMeasure>(), getMeasure<NetMeasure>(), s.isVisible(WidgetType::Time));
    case WidgetType::SystemStats:
        return std::make_unique<SystemStatsWidget>(&m_fontManager, getMeasure<CPUMeasure>(), getMeasure<GPUMeasure>(), getMeasure<DisplayMeasure>(), getMeasure<SystemMeasure>(), s.isVisible(WidgetType::SystemStats));
    case WidgetType::Music:
        return std::make_unique<MusicWidget>(&m_fontManager, getMeasure<MusicMeasure>(), s.isVisible(WidgetType::Music));
    case WidgetType::CPUStats:
        return std::make_unique<CPUStatsWidget>(&m_fontManager, getMeasure<CPUMeasure>(), s.isVisible(WidgetType::CPUStats));
    case WidgetType::HDD:
        return std::make_unique<HDDWidget>(&m_fontManager, getMeasure<DriveMeasure>(), s.isVisible(WidgetType::HDD));
    case WidgetType::Main:
        return std::make_unique<MainWidget>(&m_fontManager, getMeasure<AnimationState>(), s.isVisible(WidgetType::Main));
    case WidgetType::FPS:
        return std::make_unique<FPSWidget>(&m_fontManager, s.isVisible(WidgetType::FPS));
    case WidgetType::NetStats:
        return std::make_unique<NetStatsWidget>(&m_fontManager, getMeasure<NetMeasure>(), s.isVisible(WidgetType::NetStats));
    case WidgetType::CPUGraph:
        return std::make_unique<CPUGraphWidget>(&m_fontManager, getMeasure<CPUMeasure>(), s.isVisible(WidgetType::CPUGraph));
    case WidgetType::RAMGraph:
        return std::make_unique<RAMGraphWidget>(&m_fontManager, getMeasure<RAMMeasure>(), s.isVisible(WidgetType::RAMGraph));
    case WidgetType::NetGraph:
        return std::make_unique<NetGraphWidget>(&m_fontManager, getMeasure<NetMeasure>(), s.isVisible(WidgetType::NetGraph));
    case WidgetType::GPUGraph:
        return std::make_unique<GPUGraphWidget>(&m_fontManager, getMeasure<GPUMeasure>(), s.isVisible(WidgetType::GPUGraph));
    default:
        RGERROR("Unknown widget type.");
        break;
    }

    return nullptr;
}

std::shared_ptr<Measure> RetroGraph::createMeasure(MeasureType measureType) const {
    switch (measureType) {
    case MeasureType::CPU:
        return std::make_shared<CPUMeasure>();
    case MeasureType::GPU:
        return std::make_shared<GPUMeasure>();
    case MeasureType::RAM:
        return std::make_shared<RAMMeasure>();
    case MeasureType::Net:
        return std::make_shared<NetMeasure>();
    case MeasureType::Process:
        return std::make_shared<ProcessMeasure>();
    case MeasureType::Drive:
        return std::make_shared<DriveMeasure>();
    case MeasureType::Music:
        return std::make_shared<MusicMeasure>();
    case MeasureType::System:
        return std::make_shared<SystemMeasure>();
    case MeasureType::AnimationState:
        return std::make_shared<AnimationState>();
    case MeasureType::Display:
        return std::make_shared<DisplayMeasure>();
    default:
        RGERROR("Unknown measure type.");
        break;
    }

    return nullptr;
}

auto RetroGraph::createWidgetContainers() const {
    decltype(m_widgetContainers) widgetContainerList;
    for (auto i = int{ 0 }; i < static_cast<int>(WidgetPosition::NUM_POSITIONS); ++i)
        widgetContainerList.emplace_back(std::make_unique<WidgetContainer>(static_cast<WidgetPosition>(i)));

    return widgetContainerList;
}

auto RetroGraph::createWidgets() const {
    decltype(m_widgets) widgetList( static_cast<int>(WidgetType::NumWidgets) );

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetType::NumWidgets); ++i)
        widgetList[i] = createWidget(static_cast<WidgetType> (i));

    return widgetList;
}

RetroGraph::RetroGraph(HINSTANCE hInstance)
    : m_measures( static_cast<int>(MeasureType::NumMeasures) )
    , m_window{ this, getMeasure<DisplayMeasure>(), hInstance, UserSettings::inst().getVal<int>("Window.Monitor") }
    , m_widgetVisibilities( static_cast<int>(WidgetType::NumWidgets) )
    , m_renderTargetHandle{ m_window.getHwnd() }
    , m_fontManager{ m_window.getHwnd(), m_window.getHeight() }
    , m_widgets{ createWidgets() }
    , m_widgetContainers{ createWidgetContainers() } {

    updateWidgetVisibilities();

    setViewports(m_window.getWidth(), m_window.getHeight());

    update(0);
    draw(0);
}

RetroGraph::~RetroGraph() {
}

void RetroGraph::update(int ticks) {
    refreshConfig(ticks);

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
        (m_widgets[static_cast<int>(WidgetType::Main)]) ? getMeasure<AnimationState>()->getAnimationFPS() : 2
    };

    draw(ticks, m_window.getHwnd(), m_window.getHGLRC(), framesPerSecond);
}

void RetroGraph::refreshConfig(int ticks) {
    if (ticksMatchSeconds(ticks, 5) && UserSettings::inst().checkConfigChanged()) {
        UserSettings::inst().refresh();

        const auto oldWidgetVisibilites{ m_widgetVisibilities };
        updateWidgetVisibilities();
        for (auto i = size_t{ 0U }; i < m_widgetVisibilities.size(); ++i) {
            if (oldWidgetVisibilites[i] != m_widgetVisibilities[i]) {
                const auto w{ static_cast<WidgetType>(i) };
                setWidgetVisibility(w, m_widgetVisibilities[static_cast<int>(w)]);
            }
        }

        setViewports(m_window.getWidth(), m_window.getHeight());

        draw(0);
    }
}

void RetroGraph::updateWindowSize(int newWidth, int newHeight) {
    setViewports(newWidth, newHeight);
    m_fontManager.refreshFonts(newHeight);
}

void RetroGraph::toggleWidget(WidgetType w) {
    m_widgetVisibilities[static_cast<int>(w)] = !m_widgetVisibilities[static_cast<int>(w)];

    setWidgetVisibility(w, m_widgetVisibilities[static_cast<int>(w)]);
    setViewports(m_window.getWidth(), m_window.getHeight());
    draw(0, m_window.getHwnd(), m_window.getHGLRC(), 1);
}

void RetroGraph::shutdown() {
    UserSettings::inst().writeDataFile();
}

void RetroGraph::updateWidgetVisibilities() {
    for (auto i = size_t{ 0U }; i < static_cast<int>(WidgetType::NumWidgets); ++i)
        m_widgetVisibilities[i] = UserSettings::inst().isVisible(static_cast<WidgetType>(i));
}

void RetroGraph::draw(int ticks, HWND window, HGLRC hrc, int totalFPS) const {
    if (ticksMatchRate(ticks, 2U) || ticksMatchRate(ticks, totalFPS)) {

        auto hdc{ GetDC(window) };
        wglMakeCurrent(hdc, hrc);

        glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

        // Render the bulk of widgets at a low FPS to keep light on resources
        if (ticksMatchRate(ticks, 2U)) {
            for (const auto& widgetContainer : m_widgetContainers)
                widgetContainer->draw();

        } else if (ticksMatchRate(ticks, totalFPS)) {
            // The main widget can have a higher framerate, so call every tick
            const auto& mainWidget{ dynamic_cast<MainWidget&>(*m_widgets[static_cast<int>(WidgetType::Main)]) };
            const auto& mainWidgetContainer{ m_widgetContainers[static_cast<int>(WidgetPosition::MID_MID)] };
            if (mainWidget.needsDraw(ticks))
                mainWidgetContainer->draw();
        }

        SwapBuffers(hdc);
        ReleaseDC(window, hdc);

        FPSLimiter::inst().end();
        FPSLimiter::inst().begin();
    }
}

void RetroGraph::setWidgetVisibility(WidgetType w, bool visible) {
    auto* widget{ m_widgets[static_cast<int>(w)].get()};
    if (widget) {
        widget->setVisibility(visible);
        // Update the children of the widget container
        const auto pos{ UserSettings::inst().getWidgetPosition(w) };
        auto& widgetContainer{ m_widgetContainers[static_cast<int>(pos)] };
        if (visible) {
            widgetContainer->addChild(m_widgets[static_cast<int>(w)].get());
        } else {
            widgetContainer->removeChild(m_widgets[static_cast<int>(w)].get());
            destroyWidget(w);
        }
    } else if (visible) {
        m_widgets[static_cast<int>(w)] = createWidget(w);
        m_widgets[static_cast<int>(w)]->setVisibility(visible);
    }
}

void RetroGraph::destroyWidget(WidgetType widgetType) {
    m_widgets[static_cast<int>(widgetType)] = nullptr;
    cleanupUnusedMeasures();
}

void RetroGraph::cleanupUnusedMeasures() {
    for (auto i = int{ 0 }; i < static_cast<int>(MeasureType::NumMeasures); ++i) {
        auto& measurePtr{ m_measures[i] };
        if (measurePtr && measurePtr.use_count() == 1) {
            measurePtr.reset();
        }
    }
}

void RetroGraph::setViewports(int windowWidth, int windowHeight) {
    // Update child widgets. Their position may have changed, so they'll need to go to the correct container
    std::for_each(m_widgetContainers.begin(), m_widgetContainers.end(), [](const auto& wc) { wc->clearChildren(); });

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetType::NumWidgets); ++i) {
        const auto widgetType{ static_cast<WidgetType>(i) };
        const auto widgetPos{ UserSettings::inst().getWidgetPosition(widgetType) };
        auto* widget{ m_widgets[static_cast<int>(widgetType)].get() };
        auto& container{ *m_widgetContainers[static_cast<int>(widgetPos)] };

        if (widgetType == WidgetType::FPS)
            container.setType(ContainerType::Mini);
        else
            container.resetType();

        if (widget && widget->isVisible())
            container.addChild(widget);
    }

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetPosition::NUM_POSITIONS); ++i) {
        m_widgetContainers[i]->setViewport(windowWidth, windowHeight, static_cast<WidgetPosition>(i));
        m_widgetContainers[i]->clear();
    }
}
} // namespace rg
