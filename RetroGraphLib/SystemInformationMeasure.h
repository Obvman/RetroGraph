#pragma once

#include "Measure.h"

namespace rg {

// I forgot why I made this class... performance testing or something?
// Maybe it was meant to be a proper measure for SystemStatsWidget...

// I remember! I was doing this to get NtQueryInformation data for stuff (mainly HDD usage)
class SystemInformationMeasure : public Measure {
public:
    SystemInformationMeasure();
    ~SystemInformationMeasure() = default;

    void update(uint32_t ticks) override;

    bool shouldUpdate(uint32_t ticks) const override;
};

}
