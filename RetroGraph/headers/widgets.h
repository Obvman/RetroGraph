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

constexpr size_t RG_WIDGET_PROCESSES{ 0U };
constexpr size_t RG_WIDGET_TIME{ 1U };
constexpr size_t RG_WIDGET_SYSTEM_STATS{ 2U };
constexpr size_t RG_WIDGET_MUSIC{ 3U };
constexpr size_t RG_WIDGET_CPU_STATS{ 4U };
constexpr size_t RG_WIDGET_DRIVES{ 5U };
constexpr size_t RG_WIDGET_MAIN{ 6U };
constexpr size_t RG_WIDGET_GRAPHS{ 7U };

constexpr size_t RG_NUM_WIDGETS{ 8U };

}
