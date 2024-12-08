module RG.Measures:GPUMeasure;

namespace rg {

GPUMeasure::GPUMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<IGPUDataSource> gpuDataSource)
    : Measure{ updateInterval }
    , m_gpuDataSource{ std::move(gpuDataSource) } {}

bool GPUMeasure::updateInternal() {
    onGPUUsage.raise(m_gpuDataSource->getGPUUsage());
    return true;
}

} // namespace rg
