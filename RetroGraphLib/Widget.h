#pragma once

#include "stdafx.h"

#include "drawUtils.h"
// TODO integrate the FontManager observer

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

class FontManager;

class Widget {
public:
    Widget(const FontManager* fm, bool visibility) : 
        m_fontManager{ fm }, m_visible { visibility } { }
    virtual ~Widget() = default;

    /* Draws the widget. Must be overriden */
    virtual void draw() const = 0;

    /* Clears the widget's entire viewport */
    virtual void clear() const;

    /* Sets the viewport for the entire widget. Should be overriden
       for widgets with sub-viewports */
    virtual void setViewport(Viewport vp) { m_viewport = vp; }

    /* Turns on/off drawing of the widget */
    void setVisibility(bool b);
protected:
    bool m_visible;
    Viewport m_viewport{};
    const FontManager* m_fontManager;
};

} // namespace rg