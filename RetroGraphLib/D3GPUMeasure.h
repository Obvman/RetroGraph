#pragma once

#include "Measure.h"

namespace rg {

class D3GPUMeasure : public Measure {
public:
    D3GPUMeasure();
    ~D3GPUMeasure();

private:
    void update(uint32_t) override;

    /* Must true if the Measure should update this frame */
    bool shouldUpdate(uint32_t ticks) const override;
};


}
