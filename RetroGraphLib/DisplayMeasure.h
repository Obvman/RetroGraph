#pragma once

#include "Measure.h"

#include "Monitors.h"

namespace rg {

class FontManager;
class RetroGraph;

class DisplayMeasure : public Measure {
public:
    DisplayMeasure();
    ~DisplayMeasure() noexcept = default;

    void update(int /*ticks*/) override { /* Do nothing */ }

    const Monitors* getMonitors() const { return &m_monitors; }

    void refreshSettings() override { }

private:
    Monitors m_monitors;
};

}
