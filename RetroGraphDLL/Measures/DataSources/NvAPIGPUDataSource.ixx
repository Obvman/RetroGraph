export module RG.Measures.DataSources:NvAPIGPUDataSource;

import :IGPUDataSource;

import std.compat;

import "NvidiaHeaderUnit.h";

namespace rg {

export class NvAPIGPUDataSource : public IGPUDataSource {
public:
    NvAPIGPUDataSource();
    ~NvAPIGPUDataSource();

    const std::string& getGPUName() const override { return m_gpuName; }
    const std::string& getDriverVersion() const override { return m_driverVersion; }
    float getGPUUsage() const override;
    int getGPUTemp() const override;
    int getGPUTotalMemoryKB() const override { return m_totalMemoryKB; }
    int getGPUAvailableMemoryKB() const override;
    int getGPUFrameBufferSizeKB() const override { return m_gpuFrameBufferSizeKB; }
    int getGPUCoreCount() const override { return m_gpuCoreCount; }

private:
    NvPhysicalGpuHandle getGPUHandle() const;
    std::string determineGPUName() const;
    std::string determineDriverVersion() const;
    int determineGPUCoreCount() const;
    int determineTotalMemory() const;
    int determineGPUFrameBufferSize() const;
    int64_t determineGPURunningTimeTotal() const;

    NvPhysicalGpuHandle m_gpuHandle;
    std::string m_gpuName;
    std::string m_driverVersion;
    int m_totalMemoryKB;
    int m_gpuCoreCount;
    int m_gpuFrameBufferSizeKB;
};

} // namespace rg
