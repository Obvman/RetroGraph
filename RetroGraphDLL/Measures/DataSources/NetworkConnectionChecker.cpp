module RG.Measures.DataSources:NetworkConnectionChecker;

import "WindowsNetworkHeaderUnit.h";

#pragma comment(lib, "Wininet.lib")

namespace rg {

bool NetworkConnectionChecker::checkConnectionStatusChanged() {
    if (m_connectionStatusChanged.load()) {
        m_connectionStatusChanged.store(false);
        return true;
    }
    return false;
}

void NetworkConnectionChecker::startNetworkThread() {
    // Start thread that periodically checks connection to internet.
    using namespace std::chrono_literals;

    m_threadRunning.store(true);
    m_netConnectionThread = std::thread{ [this]() {
        while (m_threadRunning.load()) {
            std::unique_lock<std::mutex> lg{ m_netConnectionMutex };

            bool isConnected{ static_cast<bool>(
                InternetCheckConnectionA(m_pingServer.c_str(), FLAG_ICC_FORCE_CONNECTION, 0)) };
            if (m_isConnected != isConnected) {
                m_isConnected.store(isConnected);
                m_connectionStatusChanged.store(true);
            }

            m_netConnectionCV.wait_for(lg, m_pingFrequency, [&]() { return !m_threadRunning.load(); });
        }
    } };
}

void NetworkConnectionChecker::destroyNetworkThread() {
    // End the background thread
    m_threadRunning.store(false);
    m_netConnectionCV.notify_all();
    m_netConnectionThread.join();
}

} // namespace rg
