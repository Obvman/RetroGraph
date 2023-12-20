module RG.Measures:RAMMeasure;

namespace rg {

RAMMeasure::RAMMeasure(std::chrono::milliseconds updateInterval,
                       std::unique_ptr<const IRAMDataSource> ramDataSource)
    : Measure{ updateInterval }
    , m_ramDataSource{ std::move(ramDataSource) } {
}

bool RAMMeasure::updateInternal() {
    onRAMUsage.raise(m_ramDataSource->getRAMUsage());
    return true;
}

} // namespace rg
