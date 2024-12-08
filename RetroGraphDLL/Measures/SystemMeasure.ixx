export module RG.Measures:SystemMeasure;

import :Measure;

import RG.Measures.DataSources;

import std.core;

namespace rg {

export class SystemMeasure : public Measure {
public:
    SystemMeasure(std::unique_ptr<const IOperatingSystemDataSource> operatingSystemDataSource);
    ~SystemMeasure() noexcept = default;

    const std::string& getOSName() const { return m_operatingSystemData.osName; }
    const std::string& getOSVersion() const { return m_operatingSystemData.osVersion; }
    const std::string& getUserName() const { return m_operatingSystemData.userName; }
    const std::string& getComputerName() const { return m_operatingSystemData.computerName; }

protected:
    /* Contains static information about the computer so no need to update */
    bool updateInternal() override { return false; }

private:
    std::unique_ptr<const IOperatingSystemDataSource> m_operatingSystemDataSource;
    OperatingSystemData m_operatingSystemData;
};

} // namespace rg
