module RG.Measures:NetMeasure;

import RG.Core;

namespace rg {

NetMeasure::NetMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<INetDataSource> netDataSource)
    : Measure{ updateInterval }
    , m_netDataSource{ std::move(netDataSource) } {}

bool NetMeasure::updateInternal() {
    // Check if the best network interface has changed and update to the new one if so.
    static Timer updateBestInterfaceTime{ std::chrono::seconds{ 30 } };
    if (updateBestInterfaceTime.hasElapsed()) {
        m_netDataSource->updateBestAdapter();
        updateBestInterfaceTime.restart();
    }

    if (m_netDataSource->checkConnectionStatusChanged()) {
        onConnectionStatusChanged.raise(m_netDataSource->isConnected());
    }

    m_netDataSource->updateNetTraffic();
    onDownBytes.raise(m_netDataSource->getDownBytes());
    onUpBytes.raise(m_netDataSource->getUpBytes());
    return true;
}

} // namespace rg
