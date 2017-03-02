#include "../headers/GPUMeasure.h"

#include <iostream>

namespace rg {

constexpr GLint GL_GPU_MEM_TOTAL{ 0x9048 };
constexpr GLint GL_GPU_MEM_CURRENT_AVAIL { 0x9049 };

GPUMeasure::GPUMeasure() :
    m_totalGPUMemoryKB{ 0 },
    m_currAvailableGPUMemoryKB{ 0 } {

    glGetIntegerv(GL_GPU_MEM_TOTAL, &m_totalGPUMemoryKB);
    glGetIntegerv(GL_GPU_MEM_CURRENT_AVAIL, &m_currAvailableGPUMemoryKB);

    auto result{ NvAPI_Initialize() };
    if (result != NVAPI_OK) {
        fatalMessageBox("Failed to initialize NvAPI\n");
    }

    /*NvAPI_ShortString interfaceVersion;
    NvAPI_GetInterfaceVersionString(interfaceVersion);
    std::cout << interfaceVersion << '\n';*/

    NvU32 driverVersion;
    NvAPI_ShortString buildBranchStr;
    NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, buildBranchStr);

    m_driverVersion = std::to_string(driverVersion);

    // Insert a period after 3rd digit since NVIDIA drivers are formatted as xxx.xx
    m_driverVersion.insert(3, ".");
    std::cout << "Driver version: " << m_driverVersion << "\nBuild branch version: " << buildBranchStr << '\n';

    NvPhysicalGpuHandle hGpuBuff[NVAPI_MAX_LOGICAL_GPUS];
    NvU32 gpuCount;
    if (NvAPI_EnumPhysicalGPUs(hGpuBuff, &gpuCount) != NVAPI_OK) {
        fatalMessageBox("Failed to enumerate onboard GPUs");
    }
    if (gpuCount < 0) {
        fatalMessageBox("You don't have an NVIDIA GPU! Sorry I'm going to crash now");
    }

    // Just handle the first GPU in the system, I don't expect to be using multiple
    // any time soon
    m_gpuHandle = hGpuBuff[0];
    NV_GPU_THERMAL_SETTINGS_V2 thermal;
    thermal.version = NV_GPU_THERMAL_SETTINGS_VER;

    // Only receive the first thermal sensor from the GPU
    result = NvAPI_GPU_GetThermalSettings(m_gpuHandle, 0, &thermal);
    if (result != NVAPI_OK) {
        std::cout << result << '\n';
        fatalMessageBox("Failed to get thermal information from NVAPI\n");
    }
    m_currentTemp = thermal.sensor[0].currentTemp;

    std::cout << "Max temp: " << thermal.sensor[0].defaultMaxTemp << '\n';
    std::cout << "Min temp: " << thermal.sensor[0].defaultMinTemp << '\n';
    std::cout << "Current temp: " << thermal.sensor[0].currentTemp << "C\n";
}


GPUMeasure::~GPUMeasure() {
    NvAPI_Unload();
}

void GPUMeasure::update() {
    glGetIntegerv(GL_GPU_MEM_TOTAL, &m_totalGPUMemoryKB);
    glGetIntegerv(GL_GPU_MEM_CURRENT_AVAIL, &m_currAvailableGPUMemoryKB);

    // Update GPU temperature
    NV_GPU_THERMAL_SETTINGS_V2 thermal;
    thermal.version = NV_GPU_THERMAL_SETTINGS_VER;

    auto result{ NvAPI_GPU_GetThermalSettings(m_gpuHandle, 0, &thermal) };
    if (result != NVAPI_OK) {
        std::cout << result << '\n';
        fatalMessageBox("Failed to get thermal information from NVAPI\n");
    }
    m_currentTemp = thermal.sensor[0].currentTemp;
}

}