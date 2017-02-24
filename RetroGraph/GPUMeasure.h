#pragma once

#include <GL/glew.h>
#include <stdint.h>

#include "utils.h"

namespace rg {

class GPUMeasure {
public:
    GPUMeasure();
    ~GPUMeasure();

    void update();

    int32_t getTotalGPUMemoryKB() const { return m_totalGPUMemoryKB; }
    int32_t getTotalGPUMemoryMB() const { return m_totalGPUMemoryKB / KB; }
    int32_t getTotalGPUMemoryGB() const { return m_totalGPUMemoryKB / MB; }

    int32_t getCurrAvailGPUMemoryKB() const { return m_currAvailableGPUMemoryKB; }
    int32_t getCurrAvailGPUMemoryMB() const { return m_currAvailableGPUMemoryKB / KB; }
    int32_t getCurrAvailGPUMemoryGB() const { return m_currAvailableGPUMemoryKB / MB; }

    int32_t getUsedGPUMemoryKB() const { return getTotalGPUMemoryKB() - getCurrAvailGPUMemoryKB(); }
    int32_t getUsedGPUMemoryMB() const { return getTotalGPUMemoryMB() - getCurrAvailGPUMemoryMB(); }
    int32_t getUsedGPUMemoryGB() const { return getTotalGPUMemoryGB() - getCurrAvailGPUMemoryGB(); }

private:
    int32_t m_totalGPUMemoryKB;
    int32_t m_currAvailableGPUMemoryKB;
};

}