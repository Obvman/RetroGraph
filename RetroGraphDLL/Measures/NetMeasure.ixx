export module Measures.NetMeasure;

import Units;

import Measures.Measure;

import std.core;
import std.threading;

import "WindowsNetworkHeaderUnit.h";

namespace rg {

export class NetMeasure : public Measure {
public:
    NetMeasure();
    ~NetMeasure() noexcept;

    void update(int ticks) override;

    int64_t getMaxDownValue() const { return m_downMaxVal; }
    int64_t getMaxUpValue() const { return m_upMaxVal; }
    const std::vector<int64_t>& getDownData() const { return m_downBytes; }
    const std::vector<int64_t>& getUpData() const { return m_upBytes; }
    const std::string& getDNS() const { return m_DNSIP; }
    const std::string& getHostname() const { return m_hostname; }
    const std::string& getAdapterMAC() const { return m_mainAdapterMAC; }
    const std::string& getAdapterIP() const { return m_mainAdapterIP; }
    bool isConnected() const;
    void setIsConnected(bool b);

private:
    void startNetworkThread();
    void destroyNetworkThread();

    void getDNSAndHostname();
    void getMACAndLocalIP();

    _MIB_IF_TABLE2* m_table{ nullptr };
    _MIB_IF_ROW2* m_adapterEntry{ nullptr };

    std::string m_DNSIP{ "0.0.0.0" };
    std::string m_hostname{ "" };
    std::string m_mainAdapterMAC{ "00-00-00-00-00-00" };
    std::string m_mainAdapterIP{ "0.0.0.0" };

    std::string m_pingServer{ "" };
    int m_pingFreqSec{ 0U };

    std::condition_variable m_netConnectionCV;
    std::mutex m_netConnectionMutex;
    std::atomic<bool> m_isConnected{ false };
    std::atomic<bool> m_threadRunning{ false };
    std::thread m_netConnectionThread{ };

    int64_t m_downMaxVal{ 10U * GB };
    int64_t m_upMaxVal{ 10U * GB };
    size_t dataSize{ 40U };
    std::vector<int64_t> m_downBytes{ };
    std::vector<int64_t> m_upBytes{ };
};

} // namespace rg
