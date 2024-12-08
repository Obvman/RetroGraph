module RG.Measures:CPUMeasure;

import "RGAssert.h";

namespace rg {

CPUMeasure::CPUMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<ICPUDataSource> cpuDataSource)
    : Measure{ updateInterval }
    , m_cpuDataSource{ std::move(cpuDataSource) } {}

bool CPUMeasure::updateInternal() {
    m_cpuDataSource->update();

    for (int i{ 0 }; i < m_cpuDataSource->getNumCores(); ++i) {
        onCPUCoreUsage.raise(i, m_cpuDataSource->getCoreUsage(i));
    }

    onCPUUsage.raise(m_cpuDataSource->getCPUUsage());
    return true;
}

} // namespace rg
