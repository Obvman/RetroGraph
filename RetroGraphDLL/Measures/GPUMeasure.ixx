module;

#include "RGAssert.h"

#include <Windows.h> // Must be included before nvapi.h
#include <NVAPI/nvapi.h>

export module GPUMeasure;

import Measure;
import Units;
import UserSettings;
import Utils;

import std.core;

import "GLHeaders.h";
import "WindowsHeaders.h";

#pragma comment(lib, "nvapi64.lib")

namespace rg {

export class GPUMeasure;

constexpr int NVAPI_MAX_USAGES_PER_GPU{ 34 };
constexpr int NVAPI_GPU_UTILIZATION_DOMAIN_GPU{ 0U };

using NvAPI_QueryInterface_t = int *(*)(unsigned int offset);
using NvAPI_GPU_GetUsages_t = int (*)(NvPhysicalGpuHandle handle, NvU32* usages);

NvAPI_QueryInterface_t NvAPI_QueryInterface{ nullptr };
NvAPI_GPU_GetUsages_t NvAPI_GPU_GetUsages{ nullptr };

class GPUMeasure : public Measure {
public:
    GPUMeasure();
    ~GPUMeasure() noexcept;

    /* Get latest GPU stats from OpenGL or nvapi and updates dynamic members */
    void update(int ticks) override;

    void refreshSettings() override;

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

    size_t dataSize{ 40U };
    std::vector<float> m_usageData{ };
};

//void DoThing() {
//    NvAPI_Status status = NVAPI_OK;
//    NvU32 deviceCount = 0;
//    NV_DISPLAYCONFIG_PATH_INFO_V2* pathInfo = NULL;
//
//    status = NvAPI_DISP_GetDisplayConfig(&deviceCount, pathInfo);
//    if ((status == NVAPI_OK) && (deviceCount > 0)) {
//
//        printf("\nFirst pass ok. \n");
//        pathInfo = new NV_DISPLAYCONFIG_PATH_INFO_V2[deviceCount];
//        ZeroMemory(pathInfo, deviceCount * sizeof(NV_DISPLAYCONFIG_PATH_INFO_V2));
//
//        for (int i = 0; i < deviceCount; i++)
//        {
//            pathInfo[i].targetInfo = 0;
//            pathInfo[i].targetInfoCount = 0;
//            pathInfo[i].version = NV_DISPLAYCONFIG_PATH_INFO_VER2;
//            pathInfo[i].sourceModeInfo = 0;
//            pathInfo[i].reserved = 0;
//        }
//
//        status = NvAPI_DISP_GetDisplayConfig(&deviceCount, pathInfo);
//
//        if (status == NVAPI_OK) {
//
//            printf("\nSecond pass ok. \n");
//            for (int i = 0; i < deviceCount; i++)
//            {
//                pathInfo[i].sourceModeInfo = new NV_DISPLAYCONFIG_SOURCE_MODE_INFO_V1;
//                pathInfo[i].sourceModeInfo->reserved = 0;
//                pathInfo[i].targetInfo = new NV_DISPLAYCONFIG_PATH_TARGET_INFO_V2[pathInfo[i].targetInfoCount];
//                for (int j = 0; j < pathInfo[i].targetInfoCount; j++) {
//                    pathInfo[i].targetInfo[j].details = new NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_V1;
//                    pathInfo[i].targetInfo[j].details->version = NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_VER1;
//                    pathInfo[i].targetInfo[j].details->reserved = 0;
//                }
//            }
//        }
//
//        status = NvAPI_DISP_GetDisplayConfig(&deviceCount, pathInfo);
//        if (status == NVAPI_OK) {
//            printf("\nThird pass ok. \n");
//        }
//
//        for (int i = 0; i < deviceCount; i++) {
//            for (int j = 0; j < pathInfo[i].targetInfoCount; j++) {
//                switch (pathInfo[i].targetInfo[j].details->scaling) {
//                    case NV_SCALING_DEFAULT:
//                        printf("Default");
//                        break;
//                    case NV_SCALING_GPU_SCALING_TO_CLOSEST:
//                        printf("GPU Scaling to closest");
//                        break;
//                    case NV_SCALING_GPU_SCALING_TO_NATIVE:
//                        printf("GPU scaling to native");
//                        break;
//                    case NV_SCALING_GPU_SCANOUT_TO_NATIVE:
//                        printf("Gpu scanout to native");
//                        break;
//                    case NV_SCALING_GPU_SCALING_TO_ASPECT_SCANOUT_TO_NATIVE:
//                        printf("GPU scaling to aspect scanout to native");
//                        break;
//                    case NV_SCALING_GPU_SCALING_TO_ASPECT_SCANOUT_TO_CLOSEST:
//                        printf("Gpu scaling to aspect scanout to closest");
//                        break;
//                    case NV_SCALING_GPU_SCANOUT_TO_CLOSEST:
//                        printf("Gpu scanout to closest");
//                        break;
//                    case NV_SCALING_CUSTOMIZED:
//                        printf("Scaling customized");
//                    default:
//                        printf("Nothing");
//                        break;
//                }
//            }
//        }
//
//        //status = NvAPI_DISP_SetDisplayConfig(deviceCount, pathInfo, NV_DISPLAYCONFIG_VALIDATE_ONLY);
//    }
//}

GPUMeasure::GPUMeasure()
        : dataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-GPUGraph.NumUsageSamples") } {

    m_usageData.assign(dataSize, 0.0f);

    if (NvAPI_Initialize() != NVAPI_OK) {
        m_isEnabled = false;
        return;
    }

    //DoThing();

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

    m_gpuDescription = "GPU: " + m_gpuName + " (" + m_driverVersion + ")";

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
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());
}

void GPUMeasure::refreshSettings() {
    const size_t newDataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-GPUGraph.NumUsageSamples") };
    if (dataSize == newDataSize)
        return;

    m_usageData.assign(newDataSize, 0.0f);
    dataSize = newDataSize;
}

float GPUMeasure::getMemUsagePercent() const {
    return 100.0f - (static_cast<float>(m_currAvailableMemory) / m_totalMemory) * 100.0f;
}

void GPUMeasure::updateGpuTemp() {
    const auto result{ NvAPI_GPU_GetThermalSettings(m_gpuHandle, NVAPI_THERMAL_TARGET_NONE, &m_thermalSettings) };
    RGASSERT(result == NVAPI_OK, std::format("Failed to get thermal information from NVAPI: {}", static_cast<int>(result)).c_str());
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
    RGASSERT(result == NVAPI_OK, std::format("Failed to get GPU clock frequencies %d\n", static_cast<int>(result)).c_str());

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
