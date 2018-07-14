#pragma once

#include "stdafx.h"

#include <cstdint>
#include <GL/glew.h>
#include <NVAPI/nvapi.h>
#include <vector>

#include "utils.h"
#include "Measure.h"

namespace rg {

class UserSettings;

/* Stores statistics about the system's GPU Assumes the system is running
   a single, NVIDIA GPU */
class GPUMeasure : public Measure {
public:
    GPUMeasure();
    ~GPUMeasure() noexcept;
    GPUMeasure(const GPUMeasure&) = delete;
    GPUMeasure& operator=(const GPUMeasure&) = delete;
    GPUMeasure(GPUMeasure&&) = delete;
    GPUMeasure& operator=(GPUMeasure&&) = delete;

    /* Get latest GPU stats from OpenGL or nvapi and updates dynamic members */
    void update(int ticks) override;

    /* Returns true if the measure successfully initialized and is getting data,
     * And false if it failed to initialise
     */
    bool isEnabled() const { return m_isEnabled; }

    int getFrameBufferSizeKB() const { return m_frameBufferSize; }
    int getCoreCount() const { return m_gpuCoreCount; }
    int getMemoryClockHz() const { return m_memoryClock; }
    int getGraphicsClockHz() const { return m_graphicsClock; }
    int getCurrAvailableMemoryKB() const { return m_currAvailableMemory; }
    int getTotalMemoryKB() const { return m_totalMemory; }
    int getGpuUsage() const { return m_gpuUsage; }
    float getMemUsagePercent() const;
    int getCurrentTempC() const { return m_currentTemp; }
    const std::string& getDriverVersion() const { return m_driverVersion; }
    const std::string& getGpuName() const { return m_gpuName; }
    const std::string& getGpuDescription() const { return m_gpuDescription; }
    const std::vector<float>& getUsageData() const { return m_usageData; }
private:
    NvPhysicalGpuHandle getGpuHandle() const;
    void updateGpuTemp();
    void getClockFrequencies();
    void getMemInformation();
    void getGpuUsage();

    bool shouldUpdate(int ticks) const override;

    bool m_isEnabled{ true };

    std::string m_driverVersion{ "" };
    std::string m_gpuName{ "" };
    std::string m_gpuDescription{ "" };

    // NvAPI members
    NvPhysicalGpuHandle m_gpuHandle{ nullptr };
    NV_GPU_THERMAL_SETTINGS_V2 m_thermalSettings{ };
    NV_GPU_CLOCK_FREQUENCIES m_clockFreqs{ };
    NV_DISPLAY_DRIVER_MEMORY_INFO m_memInfo{ };
    NV_GPU_DYNAMIC_PSTATES_INFO_EX m_pStateInfo{ };
    NvS32 m_currentTemp{ 0 };
    NvU32 m_graphicsClock{ 0U };
    NvU32 m_memoryClock{ 0U };
    NvU32 m_gpuCoreCount{ 0U };
    NvU32 m_frameBufferSize{ 0U };
    NvU32 m_currAvailableMemory{ 0U };
    NvU32 m_totalMemory{ 0U };
    NvU32 m_gpuUsage{ 0U };

    const size_t dataSize{ 40U };
    std::vector<float> m_usageData{ };
};

} // namespace rg
