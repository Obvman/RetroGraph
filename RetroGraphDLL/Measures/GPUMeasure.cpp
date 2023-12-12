module;

#include <pdh.h>
#include <pdhmsg.h>
#include <strsafe.h>

module Measures.GPUMeasure;

import "NvidiaHeaderUnit.h";
import "RGAssert.h";

#pragma comment(lib, "nvapi64.lib")
#pragma comment(lib, "pdh.lib")

namespace rg {

constexpr int NVAPI_GPU_UTILIZATION_DOMAIN_GPU{ 0U };

GPUMeasure::GPUMeasure() {
    if (NvAPI_Initialize() != NVAPI_OK) {
        m_isEnabled = false;
        return;
    }

    NvU32 driverVersion;
    NvAPI_ShortString buildBranchStr;
    NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, buildBranchStr);

    m_driverVersion = std::to_string(driverVersion) + " " + buildBranchStr;
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

void GPUMeasure::updateInternal() {
    //updateGpuTemp(); // High CPU usage function
    //getClockFrequencies(); // High CPU usage function
    //getMemInformation();

    onGPUUsage(getGpuUsage());
    postUpdate();
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

float GPUMeasure::getGpuUsage() const {
    static auto prevCallTime{ std::chrono::steady_clock::now() };
    static int64_t prevRunningTime{ GetGpuRunningTimeTotal() };

    const std::chrono::steady_clock::time_point now{ std::chrono::steady_clock::now() };
    const std::chrono::steady_clock::duration elapsed{ now - prevCallTime };

    const int64_t elapsedNs{ std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() };
    const int64_t runningTime{ GetGpuRunningTimeTotal() };

    float percentage{ static_cast<float>(runningTime - prevRunningTime) / elapsedNs * 100 };

    prevCallTime = now;
    prevRunningTime = runningTime;

    return std::clamp(percentage, 0.0f, 1.0f);
}

int64_t GPUMeasure::GetGpuRunningTimeTotal() const {
    DWORD counterListSize{ 0 };
    DWORD instanceListSize{ 0 };
    const auto COUNTER_OBJECT{ "GPU Engine" };
    PDH_STATUS status = PdhEnumObjectItemsA(nullptr, nullptr, COUNTER_OBJECT, nullptr,
                                            &counterListSize, nullptr, &instanceListSize,
                                            PERF_DETAIL_WIZARD, 0);
    if (status != PDH_MORE_DATA) {
        RGERROR("failed PdhEnumObjectItems()");
        return 0;
    }

    std::vector<char> counterList(counterListSize);
    std::vector<char> instanceList(instanceListSize);
    status = ::PdhEnumObjectItemsA(nullptr, nullptr, COUNTER_OBJECT, counterList.data(), &counterListSize,
                                   instanceList.data(), &instanceListSize, PERF_DETAIL_WIZARD, 0);
    if (status != ERROR_SUCCESS) {
        RGERROR("failed PdhEnumObjectItems()");
        return 0;
    }

    int64_t totalRunningTime{ 0 };
    for (const char* pTemp = instanceList.data(); *pTemp != 0; pTemp += ::strlen(pTemp) + 1) {
        if (::strstr(pTemp, "engtype_3D") == nullptr) {
            continue;
        }

        char buffer[1024];
        ::StringCchCopyA(buffer, 1024, "\\GPU Engine(");
        ::StringCchCatA(buffer, 1024, pTemp);
        ::StringCchCatA(buffer, 1024, ")\\Running time");

        HQUERY hQuery{ nullptr };
        status = ::PdhOpenQueryA(nullptr, 0, &hQuery);
        if (status != ERROR_SUCCESS) {
            continue;
        }

        HCOUNTER hCounter{ nullptr };
        if (::PdhAddCounterA(hQuery, buffer, 0, &hCounter) != ERROR_SUCCESS) {
            ::PdhCloseQuery(hQuery);
            continue;
        }

        if (::PdhCollectQueryData(hQuery) != ERROR_SUCCESS) {
            ::PdhCloseQuery(hQuery);
            continue;
        }

        if (::PdhCollectQueryData(hQuery) != ERROR_SUCCESS) {
            ::PdhCloseQuery(hQuery);
            continue;
        }

        const DWORD dwFormat{ PDH_FMT_LONG };
        PDH_FMT_COUNTERVALUE ItemBuffer;
        status = ::PdhGetFormattedCounterValue(hCounter, dwFormat, nullptr, &ItemBuffer);
        if (ERROR_SUCCESS != status) {
            ::PdhCloseQuery(hQuery);
            continue;
        }
        totalRunningTime += ItemBuffer.longValue;

        ::PdhCloseQuery(hQuery);
    }

    return totalRunningTime;
}

} // namespace rg
