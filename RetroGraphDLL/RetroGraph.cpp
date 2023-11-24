module RetroGraph;

import Colors;
import FPSLimiter;
import UserSettings;

import Rendering.VBOController;

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

auto RetroGraph::createWidgetContainers() const {
    decltype(m_widgetContainers) widgetContainerList;
    for (auto i = int{ 0 }; i < static_cast<int>(WidgetPosition::NUM_POSITIONS); ++i)
        widgetContainerList.emplace_back(std::make_unique<WidgetContainer>(static_cast<WidgetPosition>(i)));

    return widgetContainerList;
}

auto RetroGraph::createWidgets() {
    decltype(m_widgets) widgetList( static_cast<int>(WidgetType::NumWidgets) );

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetType::NumWidgets); ++i) {
        if (UserSettings::inst().isVisible(static_cast<WidgetType> (i)))
            widgetList[i] = createWidget(static_cast<WidgetType> (i));
        else
            widgetList[i] = nullptr;
    }

    return widgetList;
}

RetroGraph::RetroGraph(HINSTANCE hInstance)
    : m_measures( static_cast<int>(MeasureType::NumMeasures) )
    , m_window{ this, getMeasure<DisplayMeasure>(), hInstance, UserSettings::inst().getVal<int>("Window.Monitor") }
    , m_fontManager{ m_window.getHwnd(), m_window.getHeight() }
    , m_widgets{ createWidgets() }
    , m_widgetContainers{ createWidgetContainers() } {

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
    auto framesPerSecond = int{ 2 };
    if (m_widgets[static_cast<int>(WidgetType::Main)]) {
        const auto* animationState{ m_measures[static_cast<int> (MeasureType::AnimationState)].get() };
        if (animationState) {
            framesPerSecond = dynamic_cast<const AnimationState*> (animationState)->getAnimationFPS();
        }
    }

    if (ticksMatchRate(ticks, 2U) || ticksMatchRate(ticks, framesPerSecond)) {

        auto hdc{ GetDC(m_window.getHwnd()) };
        wglMakeCurrent(hdc, m_window.getHGLRC());

        glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

        // Render the bulk of widgets at a low FPS to keep light on resources
        if (ticksMatchRate(ticks, 2U)) {
            for (const auto& widgetContainer : m_widgetContainers)
                widgetContainer->draw();

        } else if (ticksMatchRate(ticks, framesPerSecond)) {
            // The main widget can have a higher framerate, so call every tick
            const auto* mainWidget{ dynamic_cast<const MainWidget*>(m_widgets[static_cast<int>(WidgetType::Main)].get()) };
            if (mainWidget) {
                const auto& mainWidgetContainer{ m_widgetContainers[static_cast<int>(WidgetPosition::MID_MID)] };
                if (mainWidget->needsDraw(ticks))
                    mainWidgetContainer->draw();
            }
        }

        SwapBuffers(hdc);
        ReleaseDC(m_window.getHwnd(), hdc);

        FPSLimiter::inst().end();
        FPSLimiter::inst().begin();
    }
}

void RetroGraph::updateWindowSize(int newWidth, int newHeight) {
    setViewports(newWidth, newHeight);
    m_fontManager.refreshFonts(newHeight);
}

void RetroGraph::toggleWidget(WidgetType widgetType) {
    auto* widget{ m_widgets[static_cast<int>(widgetType)].get()};
    if (widget) {
        m_widgets[static_cast<int>(widgetType)] = nullptr;
        cleanupUnusedMeasures();
    } else {
        m_widgets[static_cast<int>(widgetType)] = createWidget(widgetType);
    }

    setViewports(m_window.getWidth(), m_window.getHeight());
    draw(0);
}

void RetroGraph::reloadResources() {
    refreshConfig(0);

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetType::NumWidgets); ++i) {
        if (m_widgets[i])
            m_widgets[i]->reloadShaders();
    }
}

void RetroGraph::shutdown() {
}

void RetroGraph::refreshConfig(int ticks) {
    if (ticksMatchSeconds(ticks, 5) && UserSettings::inst().checkConfigChanged()) {
        UserSettings::inst().refresh();

        for (auto i = size_t{ 0U }; i < static_cast<int>(WidgetType::NumWidgets); ++i) {
            if (static_cast<bool> (m_widgets[i]) != UserSettings::inst().isVisible(static_cast<WidgetType>(i))) {
                toggleWidget(static_cast<WidgetType>(i));
            }
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

        if (widget)
            container.addChild(widget);
    }

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetPosition::NUM_POSITIONS); ++i) {
        m_widgetContainers[i]->setViewport(windowWidth, windowHeight, static_cast<WidgetPosition>(i));
        m_widgetContainers[i]->clear();
    }
}

void RetroGraph::cleanupUnusedMeasures() {
    for (auto i = int{ 0 }; i < static_cast<int>(MeasureType::NumMeasures); ++i) {
        auto& measurePtr{ m_measures[i] };
        if (measurePtr && measurePtr.use_count() == 1) {
            measurePtr.reset();
        }
    }
}

std::unique_ptr<Widget> RetroGraph::createWidget(WidgetType widgetType) {
    switch (widgetType) {
    case WidgetType::ProcessCPU:
        return std::make_unique<ProcessCPUWidget>(&m_fontManager, getMeasure<ProcessMeasure>());
    case WidgetType::ProcessRAM:
        return std::make_unique<ProcessRAMWidget>(&m_fontManager, getMeasure<ProcessMeasure>());
    case WidgetType::Time:
        return std::make_unique<TimeWidget>(&m_fontManager, getMeasure<CPUMeasure>(), getMeasure<NetMeasure>());
    case WidgetType::SystemStats:
        return std::make_unique<SystemStatsWidget>(&m_fontManager, getMeasure<CPUMeasure>(), getMeasure<GPUMeasure>(), getMeasure<DisplayMeasure>(), getMeasure<SystemMeasure>());
    case WidgetType::Music:
        return std::make_unique<MusicWidget>(&m_fontManager, getMeasure<MusicMeasure>());
    case WidgetType::CPUStats:
        return std::make_unique<CPUStatsWidget>(&m_fontManager, getMeasure<CPUMeasure>());
    case WidgetType::HDD:
        return std::make_unique<HDDWidget>(&m_fontManager, getMeasure<DriveMeasure>());
    case WidgetType::Main:
        return std::make_unique<MainWidget>(&m_fontManager, getMeasure<AnimationState>());
    case WidgetType::FPS:
        return std::make_unique<FPSWidget>(&m_fontManager);
    case WidgetType::NetStats:
        return std::make_unique<NetStatsWidget>(&m_fontManager, getMeasure<NetMeasure>());
    case WidgetType::CPUGraph:
        return std::make_unique<CPUGraphWidget>(&m_fontManager, getMeasure<CPUMeasure>());
    case WidgetType::RAMGraph:
        return std::make_unique<RAMGraphWidget>(&m_fontManager, getMeasure<RAMMeasure>());
    case WidgetType::NetGraph:
        return std::make_unique<NetGraphWidget>(&m_fontManager, getMeasure<NetMeasure>());
    case WidgetType::GPUGraph:
        return std::make_unique<GPUGraphWidget>(&m_fontManager, getMeasure<GPUMeasure>());
    default:
        RGERROR("Unknown widget type.");
        break;
    }

    return nullptr;
}

} // namespace rg
