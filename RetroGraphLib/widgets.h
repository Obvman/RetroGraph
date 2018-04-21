#pragma once

namespace rg {

enum class WidgetPosition : uint8_t {
    TOP_LEFT,
    TOP_MID,
    TOP_RIGHT,
    MID_LEFT,
    MID_MID,
    MID_RIGHT,
    BOT_LEFT,
    BOT_MID,
    BOT_RIGHT
};

enum class WidgetType {
    Time, HDD, CPUStats, ProcessRAM, ProcessCPU, Graph, SystemStats, Main, Music, FPS
};

constexpr size_t RG_WIDGET_PROCESSES_RAM{ 0U };
constexpr size_t RG_WIDGET_PROCESSES_CPU{ 1U };
constexpr size_t RG_WIDGET_TIME{ 2U };
constexpr size_t RG_WIDGET_SYSTEM_STATS{ 3U };
constexpr size_t RG_WIDGET_MUSIC{ 4U };
constexpr size_t RG_WIDGET_CPU_STATS{ 5U };
constexpr size_t RG_WIDGET_DRIVES{ 6U };
constexpr size_t RG_WIDGET_MAIN{ 7U };
constexpr size_t RG_WIDGET_GRAPHS{ 8U };
constexpr size_t RG_WIDGET_FPS{ 9U };

constexpr size_t RG_NUM_WIDGETS{ 10U };

} // namespace rg
