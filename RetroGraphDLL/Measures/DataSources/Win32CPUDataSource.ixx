export module RG.Measures.DataSources:Win32CPUDataSource;

import :ICPUDataSource;

namespace rg {

export class Win32CPUDataSource : public ICPUDataSource {
public:
    Win32CPUDataSource();

    void update() override;

    std::string getCPUName() const override { return m_cpuName; }
    float getCPUUsage() const override { return m_cpuUsage; }
    int getNumCores() const override { return m_numCores; };
    float getCoreUsage(int /*coreIdx*/) const override { return m_cpuUsage; /*TODO*/ }
    float getClockSpeed() const override { return m_cpuClockSpeed; }
    float getVoltage() const override { return 0.0f; /*TODO*/ }
    float getTemp(int /*coreIdx*/) const override { return 0.0f; /*TODO*/ }

private:
    std::string determineCPUName() const;
    int determineNumCores() const;
    float determineClockSpeed() const; // #TODO this only returns the base clock not the current clock.
    float calculateCPUUsage() const;

    std::string m_cpuName;
    int m_numCores;
    float m_cpuUsage;
    float m_cpuClockSpeed;
};

} // namespace rg
