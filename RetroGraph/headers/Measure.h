#pragma once

#include <stdint.h>

namespace rg {

class Measure {
public:
    virtual ~Measure() = default;

    virtual void init() = 0;
    virtual void update(uint32_t ticks) = 0;
};

}
