export module RG.Measures.DataSources:NetworkConnectionChecker;

import std.core;

namespace rg {

export class NetworkConnectionChecker {
public:
    NetworkConnectionChecker(std::chrono::milliseconds pingFrequency, const std::string& pingServer)
        : m_pingFrequency{ pingFrequency }
        , m_pingServer{ pingServer }
        , m_netConnectionCV{}
        , m_netConnectionMutex{}
        , m_isConnected{ false }
        , m_connectionStatusChanged{ false }
        , m_threadRunning{ false }
        , m_netConnectionThread{} {
        startNetworkThread();
    }
    NetworkConnectionChecker() { destroyNetworkThread(); }

    bool checkConnectionStatusChanged();
    bool isConnected() const { return m_isConnected.load(); }

private:
    void startNetworkThread();
    void destroyNetworkThread();

    std::chrono::milliseconds m_pingFrequency;
    std::string m_pingServer;

    std::condition_variable m_netConnectionCV;
    std::mutex m_netConnectionMutex;
    std::atomic<bool> m_isConnected;
    std::atomic<bool> m_connectionStatusChanged;
    std::atomic<bool> m_threadRunning;
    std::thread m_netConnectionThread;
};

} // namespace rg
