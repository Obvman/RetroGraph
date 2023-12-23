module RG.Measures:SystemMeasure;

namespace rg {

SystemMeasure::SystemMeasure(std::unique_ptr<const IOperatingSystemDataSource> operatingSystemDataSource)
    : Measure{ std::nullopt }
    , m_operatingSystemDataSource{ std::move(operatingSystemDataSource) }
    , m_operatingSystemData{ m_operatingSystemDataSource->getOperatingSystemData() } {}

} // namespace rg
