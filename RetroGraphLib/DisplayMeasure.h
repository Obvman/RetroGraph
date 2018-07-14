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
    DisplayMeasure(const DisplayMeasure&) = delete;
    DisplayMeasure& operator=(const DisplayMeasure&) = delete;
    DisplayMeasure(DisplayMeasure&&) = delete;
    DisplayMeasure& operator=(DisplayMeasure&&) = delete;

    void update(int ticks) override;

    const Monitors* getMonitors() const { return &m_monitors; }

private:
    bool shouldUpdate(int ticks) const override;

    Monitors m_monitors;
};

}
