module Measures.RAMMeasure;

namespace rg {

RAMMeasure::RAMMeasure() {
    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);
}

void RAMMeasure::update(int) {
    GlobalMemoryStatusEx(&m_memStatus);

    onRAMUsage(getLoadPercentagef());
    postUpdate();
}

float RAMMeasure::getLoadPercentagef() const {
    return (static_cast<float>(getUsedPhysicalB()) / static_cast<float>(getTotalPhysicalB()));
}

} // namespace rg
