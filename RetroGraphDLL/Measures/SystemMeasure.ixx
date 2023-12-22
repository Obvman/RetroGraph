export module RG.Measures:SystemMeasure;

import :Measure;

import RG.Measures.DataSources;

import std.core;

namespace rg {

/* Contains static information about the computer so no need to update */
export class SystemMeasure : public Measure {
public:
    SystemMeasure(std::unique_ptr<const IOperatingSystemDataSource> operatingSystemDataSource);
    ~SystemMeasure() noexcept = default;

    const std::string& getOSName() const { return m_osName; }
    const std::string& getOSVersion() const { return m_osVersion; }

    /* Returns string of CPU information: Manufacturer, model, default clock
     * speed, architecture and core count */
    const std::string& getCPUDescription() const { return m_cpuDescription; }

    /* Returns the windows user name of current user */
    const std::string& getUserName() const { return m_userName; }

    const std::string& getComputerName() const { return m_computerName; }

protected:
    bool updateInternal() override { return false; }

private:
    /* Sets the contents of m_osInfoStr. Only needs to be called once */
    void getOSVersionInfo();
    /* Sets the contents of m_cpuDescription. Only needs to be called once */
    void getCPUInfo();

    std::unique_ptr<const IOperatingSystemDataSource> m_operatingSystemDataSource;

    std::string m_osName{ "" };
    std::string m_osVersion{ "" };
    std::string m_cpuDescription{ "" };
    std::string m_userName{ "" };
    std::string m_computerName{ "" };
};

} // namespace rg
