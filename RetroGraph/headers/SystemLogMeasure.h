#pragma once

#include <stdint.h>

#include "Measure.h"

namespace rg {

class SystemLogMeasure : public Measure {
public:
    SystemLogMeasure();
    virtual ~SystemLogMeasure();
    SystemLogMeasure(const SystemLogMeasure&) = delete;
    SystemLogMeasure& operator=(const SystemLogMeasure&) = delete;

    virtual void init() override;
    virtual void update(uint32_t ticks) override;

private:
};

}
