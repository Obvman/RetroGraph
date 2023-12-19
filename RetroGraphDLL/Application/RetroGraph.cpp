module RG.Application:RetroGraph;

import Colors;

import RG.Core;
import RG.Rendering;
import RG.UserSettings;

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
        widgetContainerList.emplace_back(std::make_unique<WidgetContainer>(static_cast<WidgetPosition>(i), m_drawWidgetBackgrounds));

    return widgetContainerList;
}

auto RetroGraph::createWidgets() {
    decltype(m_widgets) widgetList( static_cast<int>(WidgetType::NumWidgets) );

    const auto widgetVisibilities{createWidgetVisibilities()};
    for (auto i = int{ 0 }; i < static_cast<int>(WidgetType::NumWidgets); ++i) {
        WidgetType widgetType{ static_cast<WidgetType> (i) };
        if (widgetVisibilities[i])
            widgetList[i] = createWidget(widgetType);
        else
            widgetList[i] = nullptr;
    }

    return widgetList;
}

std::vector<bool> RetroGraph::createWidgetVisibilities() const {
    std::vector<bool> widgetVisibilities( static_cast<int>(WidgetType::NumWidgets) );

    UserSettings& settings{ UserSettings::inst() };
    widgetVisibilities[static_cast<int>(WidgetType::Time)]        = settings.getVal<bool>("Widgets-Time.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::CPUStats)]    = settings.getVal<bool>("Widgets-CPUStats.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::SystemStats)] = settings.getVal<bool>("Widgets-SystemStats.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::ProcessCPU)]  = settings.getVal<bool>("Widgets-ProcessesCPU.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::ProcessRAM)]  = settings.getVal<bool>("Widgets-ProcessesRAM.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::Music)]       = settings.getVal<bool>("Widgets-Music.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::Main)]        = settings.getVal<bool>("Widgets-Main.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::HDD)]         = settings.getVal<bool>("Widgets-Drives.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::FPS)]         = settings.getVal<bool>("Widgets-FPS.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::NetStats)]    = settings.getVal<bool>("Widgets-NetStats.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::CPUGraph)]    = settings.getVal<bool>("Widgets-CPUGraph.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::RAMGraph)]    = settings.getVal<bool>("Widgets-RAMGraph.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::NetGraph)]    = settings.getVal<bool>("Widgets-NetGraph.Visible");
    widgetVisibilities[static_cast<int>(WidgetType::GPUGraph)]    = settings.getVal<bool>("Widgets-GPUGraph.Visible");

    return widgetVisibilities;
}

auto RetroGraph::createWidgetPositions() const {
    decltype(m_widgetPositions) widgetPositions( static_cast<int>(WidgetType::NumWidgets) );

    static const std::map<std::string_view, WidgetPosition> m_posMap = {
        {"top-left",      WidgetPosition::TOP_LEFT},
        {"top-middle",    WidgetPosition::TOP_MID},
        {"top-right",     WidgetPosition::TOP_RIGHT},
        {"middle-left",   WidgetPosition::MID_LEFT},
        {"middle-middle", WidgetPosition::MID_MID},
        {"middle-right",  WidgetPosition::MID_RIGHT},
        {"bottom-left",   WidgetPosition::BOT_LEFT},
        {"bottom-middle", WidgetPosition::BOT_MID},
        {"bottom-right",  WidgetPosition::BOT_RIGHT},
    };

    UserSettings& settings{ UserSettings::inst() };
    widgetPositions[static_cast<int>(WidgetType::ProcessCPU)]  = m_posMap.at(settings.getVal<std::string>("Widgets-ProcessesCPU.Position"));
    widgetPositions[static_cast<int>(WidgetType::ProcessRAM)]  = m_posMap.at(settings.getVal<std::string>("Widgets-ProcessesRAM.Position"));
    widgetPositions[static_cast<int>(WidgetType::Time)]        = m_posMap.at(settings.getVal<std::string>("Widgets-Time.Position"));
    widgetPositions[static_cast<int>(WidgetType::SystemStats)] = m_posMap.at(settings.getVal<std::string>("Widgets-SystemStats.Position"));
    widgetPositions[static_cast<int>(WidgetType::Music)]       = m_posMap.at(settings.getVal<std::string>("Widgets-Music.Position"));
    widgetPositions[static_cast<int>(WidgetType::CPUStats)]    = m_posMap.at(settings.getVal<std::string>("Widgets-CPUStats.Position"));
    widgetPositions[static_cast<int>(WidgetType::HDD)]         = m_posMap.at(settings.getVal<std::string>("Widgets-Drives.Position"));
    widgetPositions[static_cast<int>(WidgetType::Main)]        = m_posMap.at(settings.getVal<std::string>("Widgets-Main.Position"));
    widgetPositions[static_cast<int>(WidgetType::FPS)]         = m_posMap.at(settings.getVal<std::string>("Widgets-FPS.Position"));
    widgetPositions[static_cast<int>(WidgetType::NetStats)]    = m_posMap.at(settings.getVal<std::string>("Widgets-NetStats.Position"));
    widgetPositions[static_cast<int>(WidgetType::CPUGraph)]    = m_posMap.at(settings.getVal<std::string>("Widgets-CPUGraph.Position"));
    widgetPositions[static_cast<int>(WidgetType::RAMGraph)]    = m_posMap.at(settings.getVal<std::string>("Widgets-RAMGraph.Position"));
    widgetPositions[static_cast<int>(WidgetType::NetGraph)]    = m_posMap.at(settings.getVal<std::string>("Widgets-NetGraph.Position"));
    widgetPositions[static_cast<int>(WidgetType::GPUGraph)]    = m_posMap.at(settings.getVal<std::string>("Widgets-GPUGraph.Position"));

    return widgetPositions;
}

RetroGraph::RetroGraph(HINSTANCE hInstance)
    : m_window{ this, getOrCreate(m_displayMeasure), hInstance, UserSettings::inst().getVal<int>("Window.Monitor") }
    , m_fontManager{ m_window.getHwnd(), m_window.getHeight() }
    , m_fpsCounter{}
    , m_widgetPositions(createWidgetPositions())
    , m_drawWidgetBackgrounds{ UserSettings::inst().getVal<bool>("Window.WidgetBackground") }
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

void RetroGraph::toggleWidgetBackgroundVisible() {
    m_drawWidgetBackgrounds = !m_drawWidgetBackgrounds;
    for (const auto& widgetContainer : m_widgetContainers) {
        widgetContainer->setDrawBackground(m_drawWidgetBackgrounds);
    }
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
    UserSettings& settings{ UserSettings::inst() };
    if (settings.checkConfigChanged()) {
        settings.refresh();

        const auto widgetVisibilities{createWidgetVisibilities()};

        getWidgetPropertiesFromSettings();

        // Update widget visibilities
        for (auto i = size_t{ 0U }; i < static_cast<int>(WidgetType::NumWidgets); ++i) {
            if (widgetVisibilities[i] != isWidgetVisible(static_cast<WidgetType>(i))) {
                toggleWidget(static_cast<WidgetType>(i));
            }
        }

        // Update widget background
        for (const auto& widgetContainer : m_widgetContainers) {
            widgetContainer->setDrawBackground(m_drawWidgetBackgrounds);
        }

        setViewports(m_window.getWidth(), m_window.getHeight());
    }
}

void RetroGraph::getWidgetPropertiesFromSettings() {
    m_widgetPositions = createWidgetPositions();
    m_drawWidgetBackgrounds = UserSettings::inst().getVal<bool>("Window.WidgetBackground");
}

void RetroGraph::setViewports(int windowWidth, int windowHeight) {
    // Update child widgets. Their position may have changed, so they'll need to go to the correct container
    std::for_each(m_widgetContainers.begin(), m_widgetContainers.end(), [](const auto& wc) { wc->clearChildren(); });

    for (auto i = int{ 0 }; i < static_cast<int>(WidgetType::NumWidgets); ++i) {
        const auto widgetType{ static_cast<WidgetType>(i) };
        const auto widgetPos{ getWidgetPosition(widgetType) };
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
