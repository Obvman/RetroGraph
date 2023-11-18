export module Measures.SystemMeasure;

import Measures.Measure;

import std.core;

namespace rg {

/* Contains static information about the computer so no need to update */
export class SystemMeasure : public Measure {
public:
    SystemMeasure();
    ~SystemMeasure() noexcept = default;

    void update(int ticks) override;

    /* Returns string of current operating system version/build number */
    std::string_view getOSInfoStr() const { return m_osInfoStr; }

    /* Returns string of CPU information: Manufacturer, model, default clock
     * speed, architecture and core count */
    std::string_view getCPUDescription() const { return m_cpuDescription; }

    /* Returns string with RAM capacity */
    std::string_view getRAMDescription() const { return m_ramDescription; }

    /* Returns the windows user name of current user */
    std::string_view getUserName() const { return m_userName; }

    std::string_view getComputerName() const { return m_computerName; }

private:
    bool shouldUpdate(int /*ticks*/) const override { return false; }

    /* Sets the contents of m_osInfoStr. Only needs to be called once */
    void getOSVersionInfo();
    /* Sets the contents of m_cpuDescription. Only needs to be called once */
    void getCPUInfo();
    /* Sets the contents of m_ramDescription. Only needs to be called once */
    void getRAMInfo();

    std::string m_osInfoStr{ "" };
    std::string m_cpuDescription{ "" };
    std::string m_ramDescription{ "" };
    std::string m_userName{ "" };
    std::string m_computerName{ "" };
};

} // namespace rg
