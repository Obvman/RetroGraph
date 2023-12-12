export module Measures.CPUMeasure;

import Measures.CPUPlugin;
import Measures.Measure;

import std.core;

import "EventppHeaderUnit.h";

namespace rg {

export using CPUUsageCallbackList = eventpp::CallbackList<void (float)>;
export using CPUUsageCallbackHandle = CPUUsageCallbackList::Handle;
export using CPUCoreUsageCallbackList = eventpp::CallbackList<void (int, float)>;
export using CPUCoreUsageCallbackHandle = CPUCoreUsageCallbackList::Handle;
export using CPUCoreDataStateChangedCallbackList = eventpp::CallbackList<void (bool)>;
export using CPUCoreDataStateChangedCallbackHandle = CPUCoreDataStateChangedCallbackList::Handle;

/* Measures statistics about the system CPU: Model name, total CPU load*/
export class CPUMeasure : public Measure {
public:
    CPUMeasure();
    ~CPUMeasure() noexcept;

    /* Returns the current system CPU load as a percentage */
    float getCPULoad();

    /* Creates new string containing the current system uptime in
       dd:hh:mm:ss format */
    std::string getUptimeStr() const;

    /* Returns the number of physical cores in the CPU */
    int getNumCores() const { return m_coreTempPlugin.getNumCores(); }

    /* Returns the current CPU clock speed in Megahertz */
    float getClockSpeed() const { return m_coreTempPlugin.getClockSpeed(); }

    /* Returns the current CPU voltage in volts */
    float getVoltage() const { return m_coreTempPlugin.getVoltage(); }

    /* Returns the temperature of the specified core */
    float getTemp(int coreNum) const { return m_coreTempPlugin.getTemp(coreNum); }

    /* Returns the maximum allowable CPU temperature in degrees celsius */
    int getTjMax() const { return m_coreTempPlugin.getTjMax(); }

    /* Gets description of the CPU model */
    const std::string& getCPUName() const { return m_cpuName; }

    bool getCoreTempInfoSuccess() const { return m_coreTempPlugin.getCoreTempInfoSuccess(); }

    mutable CPUUsageCallbackList onCPUUsage;
    mutable CPUCoreUsageCallbackList onCPUCoreUsage;
    mutable CPUCoreDataStateChangedCallbackList onCPUCoreDataStateChanged;

protected:
    /* Updates the total system's CPU usage statistics */
    void updateInternal() override;

    std::chrono::microseconds updateInterval() const override { return std::chrono::seconds{ 1 }; }

private:
    /* Fill CPU name if CoreTemp interfacing was successful */
    void updateCPUName();

    /* Calculates the total CPU load with the given tick information */
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);

    CPUPlugin m_coreTempPlugin{};

    std::chrono::milliseconds m_uptime{ 0 };
    std::string m_cpuName{ "" };
};

} // namespace rg

