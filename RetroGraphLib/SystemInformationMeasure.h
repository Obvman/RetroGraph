#pragma once

#include "Measure.h"

namespace rg {

class SystemInformationMeasure : public Measure {
public:
    SystemInformationMeasure();
    ~SystemInformationMeasure() = default;

    void update(uint32_t ticks) override;

    bool shouldUpdate(uint32_t ticks) const override;
};

}
