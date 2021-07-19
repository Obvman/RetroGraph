#include "stdafx.h"

#include "GPUMeasure.h"

#include <cstdio>

#include "units.h"
#include "UserSettings.h"

namespace rg {

constexpr int NVAPI_MAX_USAGES_PER_GPU{ 34 };
constexpr int NVAPI_GPU_UTILIZATION_DOMAIN_GPU{ 0U };

using NvAPI_QueryInterface_t = int *(*)(unsigned int offset);
using NvAPI_GPU_GetUsages_t = int (*)(NvPhysicalGpuHandle handle, NvU32* usages);

NvAPI_QueryInterface_t NvAPI_QueryInterface{ nullptr };
NvAPI_GPU_GetUsages_t NvAPI_GPU_GetUsages{ nullptr };

GPUMeasure::GPUMeasure()
        : dataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-GPUGraph.NumUsageSamples") } {

    m_usageData.assign(dataSize, 0.0f);

    if (NvAPI_Initialize() != NVAPI_OK) {
        m_isEnabled = false;
        return;
    }

    NvU32 driverVersion;
    NvAPI_ShortString buildBranchStr;
    NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, buildBranchStr);

    m_driverVersion = std::to_string(driverVersion);
    // Insert a period after 3rd digit since NVIDIA drivers are formatted as xxx.xx
    m_driverVersion.insert(3, ".");

    // Just handle the first GPU in the system, I don't expect to be using 
    // multiple any time soon
    m_gpuHandle = getGpuHandle();

    // Initialise static members
    NvAPI_ShortString gpuName;
    NvAPI_GPU_GetGpuCoreCount(m_gpuHandle, &m_gpuCoreCount);
    NvAPI_GPU_GetPhysicalFrameBufferSize(m_gpuHandle, &m_frameBufferSize);
    NvAPI_GPU_GetFullName(m_gpuHandle, gpuName);
    m_gpuName = gpuName;

    m_gpuDescription = "GPU: NVIDIA" + m_gpuName + " (" + m_driverVersion + ")";

    // Initialise updating member structs
    m_thermalSettings.version = NV_GPU_THERMAL_SETTINGS_VER;
    m_thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;
    m_thermalSettings.sensor[0].controller = NVAPI_THERMAL_CONTROLLER_GPU_INTERNAL;

    m_clockFreqs.version = NV_GPU_CLOCK_FREQUENCIES_VER;

    m_memInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;

    m_pStateInfo.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
}

GPUMeasure::~GPUMeasure() {
    NvAPI_Unload();
}

void GPUMeasure::update(int) {
    //updateGpuTemp(); // High CPU usage function
    //getClockFrequencies(); // High CPU usage function
    //getMemInformation();
    getGpuUsage();

    m_usageData[0] = m_gpuUsage / 100.0f;
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1,
                m_usageData.end());
}

void GPUMeasure::refreshSettings() {
    const size_t newDataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-GPUGraph.NumUsageSamples") };
    if (dataSize == newDataSize)
        return;

    m_usageData.assign(newDataSize, 0.0f);
    dataSize = newDataSize;
}

float GPUMeasure::getMemUsagePercent() const {
    return 100.0f -
        (static_cast<float>(m_currAvailableMemory) / m_totalMemory) * 100.0f;
}

void GPUMeasure::updateGpuTemp() {
    const auto result{ NvAPI_GPU_GetThermalSettings(m_gpuHandle, NVAPI_THERMAL_TARGET_NONE, &m_thermalSettings) };
    RGASSERT(result == NVAPI_OK, std::format("Failed to get thermal information from NVAPI: {}", static_cast<int>(result)));
    m_currentTemp = m_thermalSettings.sensor[0].currentTemp;
}

NvPhysicalGpuHandle GPUMeasure::getGpuHandle() const {
    NvPhysicalGpuHandle hGpuBuff[NVAPI_MAX_LOGICAL_GPUS];
    NvU32 gpuCount;

    RGVERIFY(NvAPI_EnumPhysicalGPUs(hGpuBuff, &gpuCount) == NVAPI_OK, "Failed to enumerate onboard GPUs");
    RGASSERT(gpuCount >= 0, "You don't have an NVIDIA GPU!");

    return hGpuBuff[0];
}

void GPUMeasure::getClockFrequencies() {
    const auto result{ NvAPI_GPU_GetAllClockFrequencies(m_gpuHandle, &m_clockFreqs) };
    RGASSERT(result == NVAPI_OK, std::format("Failed to get GPU clock frequencies %d\n", static_cast<int>(result)));

    m_graphicsClock = m_clockFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
    m_memoryClock = m_clockFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;
}

void GPUMeasure::getMemInformation() {
    RGVERIFY(NvAPI_GPU_GetMemoryInfo(m_gpuHandle, &m_memInfo) == NVAPI_OK, "Failed to get GPU memory information");

    m_currAvailableMemory = m_memInfo.curAvailableDedicatedVideoMemory;
    m_totalMemory = m_memInfo.availableDedicatedVideoMemory;
}

void GPUMeasure::getGpuUsage() {
    RGVERIFY(NvAPI_GPU_GetDynamicPstatesInfoEx(m_gpuHandle, &m_pStateInfo) == NVAPI_OK, "Failed to get GPU usage percentage");
    m_gpuUsage = m_pStateInfo.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_GPU].percentage;
}

}
