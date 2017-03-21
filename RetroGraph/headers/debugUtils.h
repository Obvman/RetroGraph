#pragma once

#include <stdint.h>

namespace rg {

class TimedBlock {
public:
    TimedBlock(uint32_t _id);
    ~TimedBlock();
private:
    uint32_t m_id;
};

}

