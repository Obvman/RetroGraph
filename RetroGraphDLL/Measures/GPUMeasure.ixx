export module RG.Measures:GPUMeasure;

import :Measure;

import RG.Core;

import std.core;

import "NvidiaHeaderUnit.h";

namespace rg {

export using GPUUsageEvent = CallbackEvent<float>;

export class GPUMeasure : public Measure {
public:
    GPUMeasure();
    ~GPUMeasure() noexcept;

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
    float getMemUsagePercent() const;
    int getCurrentTempC() const { return m_currentTemp; }
    const std::string& getDriverVersion() const { return m_driverVersion; }
    const std::string& getGpuName() const { return m_gpuName; }
    const std::string& getGpuDescription() const { return m_gpuDescription; }

    GPUUsageEvent onGPUUsage;

protected:
    /* Get latest GPU stats from OpenGL or nvapi and updates dynamic members */
    bool updateInternal() override;

private:
    NvPhysicalGpuHandle getGpuHandle() const;
    void updateGpuTemp();
    void getClockFrequencies();
    void getMemInformation();
    float getGpuUsage() const;
    int64_t GetGpuRunningTimeTotal() const;

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
};

} // namespace rg
