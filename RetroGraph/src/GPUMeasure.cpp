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

}


GPUMeasure::~GPUMeasure() {
    NvAPI_Unload();
}

void GPUMeasure::update() {

    m_currentTemp = updateGpuTemp();
    getClockFrequencies();
    getMemInformation();
    getGpuUsage();
}

float GPUMeasure::getMemUsagePercent() const {
    return 100.0f -
        (static_cast<float>(m_currAvailableMemory) / m_totalMemory) * 100.0f;
}

NvS32 GPUMeasure::updateGpuTemp() {
    NV_GPU_THERMAL_SETTINGS_V2 thermal;
    thermal.version = NV_GPU_THERMAL_SETTINGS_VER;
    auto result{ NvAPI_GPU_GetThermalSettings(m_gpuHandle, 0, &thermal) };
    if (result != NVAPI_OK) {
        std::cout << result << '\n';
        fatalMessageBox("Failed to get thermal information from NVAPI\n");
    }
    return thermal.sensor[0].currentTemp;
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
    NV_GPU_CLOCK_FREQUENCIES clockFreqs;
    clockFreqs.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    const auto result{ NvAPI_GPU_GetAllClockFrequencies(m_gpuHandle, &clockFreqs) };
    if (result != NVAPI_OK) {
        std::cout << result << '\n';
        fatalMessageBox("Failed to get GPU clock frequencies");
    }

    m_graphicsClock = clockFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;
    m_memoryClock = clockFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;
}

void GPUMeasure::getMemInformation() {
    NV_DISPLAY_DRIVER_MEMORY_INFO memInfo;
    memInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;
    if (NvAPI_GPU_GetMemoryInfo(m_gpuHandle, &memInfo) != NVAPI_OK) {
        fatalMessageBox("Failed to get GPU memory information");
    }

    m_currAvailableMemory = memInfo.curAvailableDedicatedVideoMemory;
    m_totalMemory = memInfo.availableDedicatedVideoMemory;
}

void GPUMeasure::getGpuUsage() {
    NV_GPU_DYNAMIC_PSTATES_INFO_EX pStateInfo;
    pStateInfo.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
    if (NvAPI_GPU_GetDynamicPstatesInfoEx(m_gpuHandle, &pStateInfo) != NVAPI_OK) {
        fatalMessageBox("Failed to get GPU usage percentage");
    }
    m_gpuUsage = pStateInfo.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_GPU].percentage;
}

}