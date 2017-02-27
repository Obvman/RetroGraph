#pragma once

#include <GL/glew.h>
#include <stdint.h>

#include "utils.h"

namespace rg {

/* Stores statistics about the system's GPU */
class GPUMeasure {
public:
    GPUMeasure();
    ~GPUMeasure();

    /* Get latest GPU stats from OpenGL */
    void update();

    int32_t getTotalGPUMemoryKB() const { return m_totalGPUMemoryKB; }

    int32_t getCurrAvailGPUMemoryKB() const { return m_currAvailableGPUMemoryKB; }

    int32_t getUsedGPUMemoryKB() const { return getTotalGPUMemoryKB() - getCurrAvailGPUMemoryKB(); }

private:
    int32_t m_totalGPUMemoryKB;
    int32_t m_currAvailableGPUMemoryKB;
};

}