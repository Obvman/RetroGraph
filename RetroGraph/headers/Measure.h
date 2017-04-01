#pragma once

#include <stdint.h>

namespace rg {

class Measure {
public:
    Measure() = default;
    virtual ~Measure() = default;
    Measure(const Measure&) = delete;
    Measure& operator=(const Measure&) = delete;

    virtual void update(uint32_t ticks) = 0;
};

}
