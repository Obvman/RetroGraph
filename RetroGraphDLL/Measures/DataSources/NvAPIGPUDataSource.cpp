module RG.Measures.DataSources:NvAPIGPUDataSource;

import "NvidiaHeaderUnit.h";
import "RGAssert.h";
import "WindowsHeaderUnit.h";

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "nvapi64.lib")

namespace rg {

NvAPIGPUDataSource::NvAPIGPUDataSource()
    : m_gpuHandle{ getGPUHandle() }
    , m_gpuName{ determineGPUName() }
    , m_driverVersion{ determineDriverVersion() }
    , m_totalMemoryKB{ determineTotalMemory() }
    , m_gpuCoreCount{ determineGPUCoreCount() }
    , m_gpuFrameBufferSizeKB{ determineGPUFrameBufferSize() } {
    if (NvAPI_Initialize() != NVAPI_OK) {
        return;
    }
}

NvAPIGPUDataSource::~NvAPIGPUDataSource() {
    NvAPI_Unload();
}

float NvAPIGPUDataSource::getGPUUsage() const {
    static auto prevCallTime{ std::chrono::steady_clock::now() };
    static int64_t prevRunningTime{ determineGPURunningTimeTotal() };

    const std::chrono::steady_clock::time_point now{ std::chrono::steady_clock::now() };
    const std::chrono::steady_clock::duration elapsed{ now - prevCallTime };

    const int64_t elapsedNs{ std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() };
    const int64_t runningTime{ determineGPURunningTimeTotal() };

    float percentage{ static_cast<float>(runningTime - prevRunningTime) / elapsedNs * 100 };

    prevCallTime = now;
    prevRunningTime = runningTime;

    return std::clamp(percentage, 0.0f, 1.0f);
}

int NvAPIGPUDataSource::getGPUTemp() const {
    NV_GPU_THERMAL_SETTINGS_V2 thermalSettings;
    thermalSettings.version = NV_GPU_THERMAL_SETTINGS_VER;
    thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;
    thermalSettings.sensor[0].controller = NVAPI_THERMAL_CONTROLLER_GPU_INTERNAL;

    const auto result{ NvAPI_GPU_GetThermalSettings(m_gpuHandle, NVAPI_THERMAL_TARGET_NONE, &thermalSettings) };
    RGASSERT(result == NVAPI_OK,
             std::format("Failed to get thermal information from NVAPI: {}", static_cast<int>(result)).c_str());
    return thermalSettings.sensor[0].currentTemp;
}

int NvAPIGPUDataSource::getGPUAvailableMemoryKB() const {
    NV_DISPLAY_DRIVER_MEMORY_INFO memInfo;
    memInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;
    RGVERIFY(NvAPI_GPU_GetMemoryInfo(m_gpuHandle, &memInfo) == NVAPI_OK, "Failed to get GPU memory information");
    return memInfo.curAvailableDedicatedVideoMemory;
}

NvPhysicalGpuHandle NvAPIGPUDataSource::getGPUHandle() const {
    NvPhysicalGpuHandle hGpuBuff[NVAPI_MAX_LOGICAL_GPUS];
    NvU32 gpuCount;
    if (NvAPI_EnumPhysicalGPUs(hGpuBuff, &gpuCount) != NVAPI_OK || gpuCount <= 0) {
        RGERROR("You don't have an NVIDIA GPU!");
        return nullptr;
    }

    return hGpuBuff[0];
}

std::string NvAPIGPUDataSource::determineGPUName() const {
    NvAPI_ShortString gpuName;
    NvAPI_GPU_GetFullName(m_gpuHandle, gpuName);
    return gpuName;
}

std::string NvAPIGPUDataSource::determineDriverVersion() const {
    NvU32 driverVersion;
    NvAPI_ShortString buildBranchStr;
    NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, buildBranchStr);

    std::string driverVersionStr = std::to_string(driverVersion) + " " + buildBranchStr;
    // Insert a period after 3rd digit since NVIDIA drivers are formatted as xxx.xx
    driverVersionStr.insert(3, ".");

    return driverVersionStr;
}

int NvAPIGPUDataSource::determineGPUCoreCount() const {
    NvU32 coreCount;
    NvAPI_GPU_GetGpuCoreCount(m_gpuHandle, &coreCount);
    return coreCount;
}

int NvAPIGPUDataSource::determineTotalMemory() const {
    NV_DISPLAY_DRIVER_MEMORY_INFO memInfo;
    memInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;
    RGVERIFY(NvAPI_GPU_GetMemoryInfo(m_gpuHandle, &memInfo) == NVAPI_OK, "Failed to get GPU memory information");
    return memInfo.availableDedicatedVideoMemory;
}

int NvAPIGPUDataSource::determineGPUFrameBufferSize() const {
    NvU32 frameBufferSize;
    NvAPI_GPU_GetPhysicalFrameBufferSize(m_gpuHandle, &frameBufferSize);
    return frameBufferSize;
}

int64_t NvAPIGPUDataSource::determineGPURunningTimeTotal() const {
    DWORD counterListSize{ 0 };
    DWORD instanceListSize{ 0 };
    const auto COUNTER_OBJECT{ "GPU Engine" };
    PDH_STATUS status = PdhEnumObjectItemsA(nullptr, nullptr, COUNTER_OBJECT, nullptr, &counterListSize, nullptr,
                                            &instanceListSize, PERF_DETAIL_WIZARD, 0);
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

        HQUERY hQuery{ nullptr };
        status = ::PdhOpenQueryA(nullptr, 0, &hQuery);
        if (status != ERROR_SUCCESS) {
            continue;
        }

        const std::string gpuRuntimeCounterStr{ std::format("\\GPU Engine({})\\Running time", pTemp) };
        HCOUNTER hCounter{ nullptr };
        if (::PdhAddCounterA(hQuery, gpuRuntimeCounterStr.c_str(), 0, &hCounter) != ERROR_SUCCESS) {
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
