export module RG.Widgets:WidgetType;

namespace rg {

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

} // namespace rg
