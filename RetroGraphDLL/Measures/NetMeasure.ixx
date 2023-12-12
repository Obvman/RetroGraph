export module Measures.NetMeasure;

import Units;
import UserSettings;

import Measures.Measure;

import std.core;
import std.threading;

import "EventppHeaderUnit.h";
import "WindowsNetworkHeaderUnit.h";

namespace rg {

export using NetUsageCallbackList = eventpp::CallbackList<void (int64_t)>;
export using NetUsageCallbackHandle = NetUsageCallbackList::Handle;

export class NetMeasure : public Measure {
public:
    NetMeasure();
    ~NetMeasure() noexcept;

    const std::string& getDNS() const { return m_DNSIP; }
    const std::string& getHostname() const { return m_hostname; }
    const std::string& getAdapterMAC() const { return m_mainAdapterMAC; }
    const std::string& getAdapterIP() const { return m_mainAdapterIP; }
    bool isConnected() const;
    void setIsConnected(bool b);

    mutable NetUsageCallbackList onDownBytes;
    mutable NetUsageCallbackList onUpBytes;

protected:
    void updateInternal() override;

    std::chrono::microseconds updateInterval() const override { return std::chrono::seconds{ 1 }; }

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

    ConfigRefreshedCallbackHandle m_configRefreshedHandle;
};

} // namespace rg
