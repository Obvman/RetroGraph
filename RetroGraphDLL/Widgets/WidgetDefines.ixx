export module Widgets.WidgetDefines;

namespace rg {

export enum class WidgetPosition : int {
    TOP_LEFT  = 0,
    TOP_MID   = 1,
    TOP_RIGHT = 2,
    MID_LEFT  = 3,
    MID_MID   = 4,
    MID_RIGHT = 5,
    BOT_LEFT  = 6,
    BOT_MID   = 7,
    BOT_RIGHT = 8,

    NUM_POSITIONS
};

export enum class WidgetType : int { // Must be consecutive integers!
    ProcessCPU  = 0,
    ProcessRAM  = 1,
    Time        = 2,
    SystemStats = 3,
    Music       = 4,
    CPUStats    = 5,
    HDD         = 6,
    Main        = 7,
    FPS         = 8,
    NetStats    = 9,
    CPUGraph    = 10,
    RAMGraph    = 11,
    NetGraph    = 12,
    GPUGraph    = 13,
    
    NumWidgets
};

}
