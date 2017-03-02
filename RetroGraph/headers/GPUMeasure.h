#pragma once

#include <stdint.h>
#include <GL/glew.h>
#include <NVAPI/nvapi.h>

#include "utils.h"

namespace rg {

/* Stores statistics about the system's GPU
   Assumes the system is running a single, NVIDIA GPU */
class GPUMeasure {
public:
    GPUMeasure();
    ~GPUMeasure();

    /* Get latest GPU stats from OpenGL */
    void update();

    int32_t getTotalGPUMemoryKB() const { return m_totalGPUMemoryKB; }
    int32_t getCurrAvailGPUMemoryKB() const { return m_currAvailableGPUMemoryKB; }
    int32_t getUsedGPUMemoryKB() const { return getTotalGPUMemoryKB() - getCurrAvailGPUMemoryKB(); }

    int32_t getCurrentTemp() const { return m_currentTemp; }
    const std::string& getDriverVersion() const { return m_driverVersion; }
private:
    int32_t m_totalGPUMemoryKB;
    int32_t m_currAvailableGPUMemoryKB;

    std::string m_driverVersion;

    // NvAPI members
    NvPhysicalGpuHandle m_gpuHandle;
    NvS32 m_currentTemp;
};

}