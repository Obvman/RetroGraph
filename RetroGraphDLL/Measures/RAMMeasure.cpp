module Measures.RAMMeasure;

import UserSettings;

namespace rg {

RAMMeasure::RAMMeasure()
    : dataSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") } {

    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);

    m_usageData.assign(dataSize, 0.0f);
}

void RAMMeasure::update(int) {
    GlobalMemoryStatusEx(&m_memStatus);

    // Add value to the list of load values and shift the list left
    m_usageData.front() = getLoadPercentagef();
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());
}

void RAMMeasure::refreshSettings() {
    const int newDataSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") };
    if (dataSize == newDataSize)
        return;

    m_usageData.assign(newDataSize, 0.0f);
    dataSize = newDataSize;
}

float RAMMeasure::getLoadPercentagef() const {
    return (static_cast<float>(getUsedPhysicalB()) /
            static_cast<float>(getTotalPhysicalB()));
}

} // namespace rg
