export module RG.Measures:GPUMeasure;

import :Measure;

import RG.Core;
import RG.Measures.DataSources;

import std.core;

namespace rg {

export using GPUUsageEvent = CallbackEvent<float>;

export class GPUMeasure : public Measure {
public:
    GPUMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<IGPUDataSource> gpuDataSource);

    int getGPUFrameBufferSizeKB() const { return m_gpuDataSource->getGPUFrameBufferSizeKB(); }
    int getGPUCoreCount() const { return m_gpuDataSource->getGPUCoreCount(); }
    int getGPUAvailableMemoryKB() const { return m_gpuDataSource->getGPUAvailableMemoryKB(); }
    int getGPUTotalMemoryKB() const { return m_gpuDataSource->getGPUTotalMemoryKB(); }
    int getGPUTemp() const { return m_gpuDataSource->getGPUTemp(); }
    const std::string& getDriverVersion() const { return m_gpuDataSource->getDriverVersion(); }
    const std::string& getGPUName() const { return m_gpuDataSource->getGPUName(); }

    GPUUsageEvent onGPUUsage;

protected:
    /* Get latest GPU stats from OpenGL or nvapi and updates dynamic members */
    bool updateInternal() override;

private:
    std::unique_ptr<IGPUDataSource> m_gpuDataSource;
};

} // namespace rg
