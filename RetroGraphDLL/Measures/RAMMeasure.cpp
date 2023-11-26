module Measures.RAMMeasure;

namespace rg {

RAMMeasure::RAMMeasure()
    : m_dataSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") }
    , m_configChangedHandle{
        UserSettings::inst().configChanged.append(
            [&]() {
                const int newDataSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") };
                if (m_dataSize != newDataSize) {
                    m_usageData.assign(newDataSize, 0.0f);
                    m_dataSize = newDataSize;
                }
            })
    } {

    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);

    m_usageData.assign(m_dataSize, 0.0f);
}

RAMMeasure::~RAMMeasure() {
    UserSettings::inst().configChanged.remove(m_configChangedHandle);
}

void RAMMeasure::update(int) {
    GlobalMemoryStatusEx(&m_memStatus);

    // Add value to the list of load values and shift the list left
    m_usageData.front() = getLoadPercentagef();
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());

    postUpdate();
}

float RAMMeasure::getLoadPercentagef() const {
    return (static_cast<float>(getUsedPhysicalB()) /
            static_cast<float>(getTotalPhysicalB()));
}

} // namespace rg
