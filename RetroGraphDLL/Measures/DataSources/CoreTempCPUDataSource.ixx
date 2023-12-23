export module RG.Measures.DataSources:CoreTempCPUDataSource;

import :ICPUDataSource;

import "CoreTempHeaderUnit.h";
import "WindowsHeaderUnit.h";

namespace rg {

/* CoreTempCPUDataSource interfaces with CoreTemp's shared memory to get more detailed
   statistics on the system's CPU: Clock speed, core count, voltage, model name,
   individual core temperatures and loads */
export class CoreTempCPUDataSource : public ICPUDataSource {
public:
    CoreTempCPUDataSource();
    ~CoreTempCPUDataSource();

    void update() override;

    std::string getCPUName() const { return m_ctData.sCPUName; }
    float getCPUUsage() const override;
    int getNumCores() const override { return m_ctData.uiCoreCnt; };
    float getCoreUsage(int coreIdx) const override { return m_ctData.uiLoad[coreIdx] / 100.0f; }
    float getClockSpeed() const override { return m_ctData.fCPUSpeed; }
    float getVoltage() const override { return m_ctData.fVID; }
    float getTemp(int coreIdx) const override { return m_ctData.fTemp[coreIdx]; }

private:
    std::string m_cpuName;
    HMODULE m_libHandle;
    CORE_TEMP_SHARED_DATA m_ctData;
};

} // namespace rg
