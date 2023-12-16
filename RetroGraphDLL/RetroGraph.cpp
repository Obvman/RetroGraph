module RetroGraph;

import Colors;
import UserSettings;

import Core.Time;

import Rendering.Shader;

import Widgets.CPUGraphWidget;
import Widgets.CPUStatsWidget;
import Widgets.FPSWidget;
import Widgets.GPUGraphWidget;
import Widgets.HDDWidget;
import Widgets.MainWidget;
import Widgets.MusicWidget;
import Widgets.NetGraphWidget;
import Widgets.NetStatsWidget;
import Widgets.ProcessCPUWidget;
import Widgets.ProcessRAMWidget;
import Widgets.RAMGraphWidget;
import Widgets.SystemStatsWidget;
import Widgets.TimeWidget;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

// TODO measure and data source factories
template<std::derived_from<Measure> T>
std::shared_ptr<T> createMeasure() {
    if constexpr (std::is_same_v<T, MusicMeasure>) {
        return std::make_shared<T>(std::make_unique<FoobarMusicDataSource>());
    } else {
        return std::make_shared<T>();
    }
}

template<std::derived_from<Measure> T>
std::shared_ptr<const T> getOrCreate(std::shared_ptr<T>& measure) {
    if (!measure)
        measure = createMeasure<T>();
    return dynamic_pointer_cast<const T> (measure);
}

template<std::derived_from<Measure> T>
void resetIfOnlyOwner(std::shared_ptr<T>& measure) {
    if (measure && measure.use_count() == 1) {
        measure.reset();
    }
}

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
    : m_window{ this, getOrCreate(m_displayMeasure), hInstance, UserSettings::inst().getVal<int>("Window.Monitor") }
    , m_fontManager{ m_window.getHwnd(), m_window.getHeight() }
    , m_fpsCounter{}
    , m_widgets{ createWidgets() }
    , m_widgetContainers{ createWidgetContainers() } {

    setViewports(m_window.getWidth(), m_window.getHeight());

    update();
    draw();
}

RetroGraph::~RetroGraph() {
}

void RetroGraph::run() {
    FPSLimiter fpsLimiter;

    // Enter main update/draw loop
    while (isRunning()) {
        // Handle Windows messages
        MSG msg{};
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        update();
        draw();

        // Lay off the CPU a little
        fpsLimiter.endFrame();

        m_fpsCounter.endFrame();
        m_fpsCounter.startFrame();
        fpsLimiter.startFrame();
    }
}

void RetroGraph::update() {
    tryRefreshConfig();

    if (m_cpuMeasure) m_cpuMeasure->update();
    if (m_gpuMeasure) m_gpuMeasure->update();
    if (m_ramMeasure) m_ramMeasure->update();
    if (m_netMeasure) m_netMeasure->update();
    if (m_processMeasure) m_processMeasure->update();
    if (m_driveMeasure) m_driveMeasure->update();
    if (m_musicMeasure) m_musicMeasure->update();
    if (m_systemMeasure) m_systemMeasure->update();
    if (m_animationState) m_animationState->update();
    if (m_displayMeasure) m_displayMeasure->update();
    if (m_timeMeasure) m_timeMeasure->update();
}

void RetroGraph::draw() const {
    auto hdc{ GetDC(m_window.getHwnd()) };
    wglMakeCurrent(hdc, m_window.getHGLRC());

    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    for (const auto& widgetContainer : m_widgetContainers)
        widgetContainer->draw();

    SwapBuffers(hdc);
    ReleaseDC(m_window.getHwnd(), hdc);
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
    draw();
}

void RetroGraph::reloadResources() {
    Shader::requestShaderRefresh();
    refreshConfig();
}

void RetroGraph::shutdown() {
}

void RetroGraph::tryRefreshConfig() {
    static Timer configChangedUpdateTimer{ std::chrono::seconds{ 5 } };
    if (configChangedUpdateTimer.hasElapsed()) {
        refreshConfig();
        configChangedUpdateTimer.restart();
    }
}

void RetroGraph::refreshConfig() {
    if (UserSettings::inst().checkConfigChanged()) {
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
    }
}

void RetroGraph::cleanupUnusedMeasures() {
    resetIfOnlyOwner(m_cpuMeasure);
    resetIfOnlyOwner(m_gpuMeasure);
    resetIfOnlyOwner(m_ramMeasure);
    resetIfOnlyOwner(m_netMeasure);
    resetIfOnlyOwner(m_processMeasure);
    resetIfOnlyOwner(m_driveMeasure);
    resetIfOnlyOwner(m_musicMeasure);
    resetIfOnlyOwner(m_systemMeasure);
    resetIfOnlyOwner(m_animationState);
    resetIfOnlyOwner(m_displayMeasure);
    resetIfOnlyOwner(m_timeMeasure);
}

std::unique_ptr<Widget> RetroGraph::createWidget(WidgetType widgetType) {
    switch (widgetType) {
    case WidgetType::ProcessCPU:
        return std::make_unique<ProcessCPUWidget>(&m_fontManager, getOrCreate(m_processMeasure));
    case WidgetType::ProcessRAM:
        return std::make_unique<ProcessRAMWidget>(&m_fontManager, getOrCreate(m_processMeasure));
    case WidgetType::Time:
        return std::make_unique<TimeWidget>(&m_fontManager, getOrCreate(m_timeMeasure), getOrCreate(m_netMeasure));
    case WidgetType::SystemStats:
        return std::make_unique<SystemStatsWidget>(&m_fontManager, getOrCreate(m_cpuMeasure), getOrCreate(m_gpuMeasure), getOrCreate(m_displayMeasure), getOrCreate(m_systemMeasure));
    case WidgetType::Music:
        return std::make_unique<MusicWidget>(&m_fontManager, getOrCreate(m_musicMeasure));
    case WidgetType::CPUStats:
        return std::make_unique<CPUStatsWidget>(&m_fontManager, getOrCreate(m_cpuMeasure));
    case WidgetType::HDD:
        return std::make_unique<HDDWidget>(&m_fontManager, getOrCreate(m_driveMeasure));
    case WidgetType::Main:
        return std::make_unique<MainWidget>(&m_fontManager, getOrCreate(m_animationState));
    case WidgetType::FPS:
        return std::make_unique<FPSWidget>(&m_fontManager, m_fpsCounter);
    case WidgetType::NetStats:
        return std::make_unique<NetStatsWidget>(&m_fontManager, getOrCreate(m_netMeasure));
    case WidgetType::CPUGraph:
        return std::make_unique<CPUGraphWidget>(&m_fontManager, getOrCreate(m_cpuMeasure));
    case WidgetType::RAMGraph:
        return std::make_unique<RAMGraphWidget>(&m_fontManager, getOrCreate(m_ramMeasure));
    case WidgetType::NetGraph:
        return std::make_unique<NetGraphWidget>(&m_fontManager, getOrCreate(m_netMeasure));
    case WidgetType::GPUGraph:
        return std::make_unique<GPUGraphWidget>(&m_fontManager, getOrCreate(m_gpuMeasure));
    default:
        RGERROR("Unknown widget type.");
        break;
    }

    return nullptr;
}

} // namespace rg
