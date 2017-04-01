#include "../headers/GPUMeasure.h"

#include <iostream>

namespace rg {

constexpr uint32_t NVAPI_MAX_USAGES_PER_GPU{ 34 };
constexpr uint32_t NVAPI_GPU_UTILIZATION_DOMAIN_GPU{ 0U };

typedef int *(*NvAPI_QueryInterface_t)(uint32_t offset);
typedef int (*NvAPI_GPU_GetUsages_t)(NvPhysicalGpuHandle handle, NvU32* usages);

NvAPI_QueryInterface_t NvAPI_QueryInterface{ nullptr };
NvAPI_GPU_GetUsages_t NvAPI_GPU_GetUsages{ nullptr };

GPUMeasure::GPUMeasure() {
    m_usageData.assign(dataSize, 0.0f);

    auto result{ NvAPI_Initialize() };
    if (result != NVAPI_OK) {
        fatalMessageBox("Failed to initialize NvAPI\n");
    }

    NvU32 driverVersion;
    NvAPI_ShortString buildBranchStr;
    NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, buildBranchStr);

    m_driverVersion = std::to_string(driverVersion);
    // Insert a period after 3rd digit since NVIDIA drivers are formatted as xxx.xx
    m_driverVersion.insert(3, ".");

    // Just handle the first GPU in the system, I don't expect to be using multiple
    // any time soon
    m_gpuHandle = getGpuHandle();

    // Initialise static members
    NvAPI_ShortString gpuName;
    NvAPI_GPU_GetGpuCoreCount(m_gpuHandle, &m_gpuCoreCount);
    NvAPI_GPU_GetPhysicalFrameBufferSize(m_gpuHandle, &m_frameBufferSize);
    NvAPI_GPU_GetFullName(m_gpuHandle, gpuName);
    m_gpuName = gpuName;

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

void GPUMeasure::update(uint32_t ticks) {
    if (ticks % (ticksPerSecond / 2) == 0) {
        //updateGpuTemp(); // High CPU usage function
        //getClockFrequencies(); // High CPU usage function
        //getMemInformation();
        getGpuUsage();

        m_usageData[0] = m_gpuUsage / 100.0f;
        std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());
    }
}

float GPUMeasure::getMemUsagePercent() const {
    return 100.0f -
        (static_cast<float>(m_currAvailableMemory) / m_totalMemory) * 100.0f;
}

void GPUMeasure::updateGpuTemp() {
    auto result{ NvAPI_GPU_GetThermalSettings(m_gpuHandle, NVAPI_THERMAL_TARGET_NONE, &m_thermalSettings) };
    if (result != NVAPI_OK) {
        std::cout << "Failed to get thermal information from NVAPI: " << result << '\n';
        return;
    }
    m_currentTemp = m_thermalSettings.sensor[0].currentTemp;
}

NvPhysicalGpuHandle GPUMeasure::getGpuHandle() const {
    NvPhysicalGpuHandle hGpuBuff[NVAPI_MAX_LOGICAL_GPUS];
    NvU32 gpuCount;
    if (NvAPI_EnumPhysicalGPUs(hGpuBuff, &gpuCount) != NVAPI_OK) {
        fatalMessageBox("Failed to enumerate onboard GPUs");
    }
    if (gpuCount < 0) {
        fatalMessageBox("You don't have an NVIDIA GPU! Sorry I'm going to crash now");
    }

    return hGpuBuff[0];
}

void GPUMeasure::getClockFrequencies() {
    const auto result{ NvAPI_GPU_GetAllClockFrequencies(m_gpuHandle, &m_clockFreqs) };
    if (result != NVAPI_OK) {
        std::cout << "Failed to get GPU clock frequencies" << result << '\n';
        return;
    }

    m_graphicsClock = m_clockFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
    m_memoryClock = m_clockFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;
}

void GPUMeasure::getMemInformation() {
    if (NvAPI_GPU_GetMemoryInfo(m_gpuHandle, &m_memInfo) != NVAPI_OK) {
        std::cout << "Failed to get GPU memory information\n";
        return;
    }

    m_currAvailableMemory = m_memInfo.curAvailableDedicatedVideoMemory;
    m_totalMemory = m_memInfo.availableDedicatedVideoMemory;
}

void GPUMeasure::getGpuUsage() {
    if (NvAPI_GPU_GetDynamicPstatesInfoEx(m_gpuHandle, &m_pStateInfo) != NVAPI_OK) {
        std::cout << "Failed to get GPU usage percentage\n";
        return;
    }
    m_gpuUsage = m_pStateInfo.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_GPU].percentage;
}

}
