#pragma once

#include "Measure.h"

#include "Monitors.h"

namespace rg {

class FontManager;
class RetroGraph;

// TODO the WINDOW is dependant on this measure! it should NEVER be destroyed!!!!!!!!
// Edit the dependency map to prevent destruction!!!!!!
class DisplayMeasure : public Measure {
public:
    DisplayMeasure(const Monitors* monitors); // TODO remove monitors
    ~DisplayMeasure() noexcept = default;
    DisplayMeasure(const DisplayMeasure&) = delete;
    DisplayMeasure& operator=(const DisplayMeasure&) = delete;
    DisplayMeasure(DisplayMeasure&&) = delete;
    DisplayMeasure& operator=(DisplayMeasure&&) = delete;

    void update(uint32_t ticks) override;

    const Monitors* getMonitors() const { return &m_monitors; }

private:
    bool shouldUpdate(uint32_t ticks) const override;

    Monitors m_monitors;
};

}
