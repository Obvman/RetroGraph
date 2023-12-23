export module RG.Measures:CPUMeasure;

import :Measure;

import RG.Core;
import RG.Measures.DataSources;

import std.core;

namespace rg {

export using CPUUsageEvent = CallbackEvent<float>;
export using CPUCoreUsageEvent = CallbackEvent<int, float>;

/* Measures statistics about the system CPU: Model name, total CPU load*/
export class CPUMeasure : public Measure {
public:
    CPUMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<ICPUDataSource> cpuDataSource);

    /* Returns the number of physical cores in the CPU */
    int getNumCores() const { return m_cpuDataSource->getNumCores(); }

    /* Returns the current CPU clock speed in Megahertz */
    float getClockSpeed() const { return m_cpuDataSource->getClockSpeed(); }

    /* Returns the current CPU voltage in volts */
    float getVoltage() const { return m_cpuDataSource->getVoltage(); }

    /* Returns the temperature of the specified core */
    float getTemp(int coreNum) const { return m_cpuDataSource->getTemp(coreNum); }

    /* Gets description of the CPU model */
    std::string getCPUName() const { return m_cpuDataSource->getCPUName(); }

    CPUUsageEvent onCPUUsage;
    CPUCoreUsageEvent onCPUCoreUsage;

protected:
    bool updateInternal() override;

private:
    std::unique_ptr<ICPUDataSource> m_cpuDataSource;
};

} // namespace rg
