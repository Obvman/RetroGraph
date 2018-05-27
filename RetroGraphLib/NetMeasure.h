#pragma once

#include "stdafx.h"

#include <cstdint>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

#include "units.h"
#include "Measure.h"

struct _MIB_IF_ROW2;
struct _MIB_IF_TABLE2;

namespace rg {

class UserSettings;

class NetMeasure : public Measure {
public:
    NetMeasure();
    ~NetMeasure() noexcept;
    NetMeasure(const NetMeasure&) = delete;
    NetMeasure& operator=(const NetMeasure&) = delete;
    NetMeasure(NetMeasure&&) = delete;
    NetMeasure& operator=(NetMeasure&&) = delete;

    void update(uint32_t ticks) override;

    uint64_t getMaxDownValue() const { return m_downMaxVal; }
    uint64_t getMaxUpValue() const { return m_upMaxVal; }
    const std::vector<uint64_t>& getDownData() const { return m_downBytes; }
    const std::vector<uint64_t>& getUpData() const { return m_upBytes; }
    const std::string& getDNS() const { return m_DNSIP; }
    const std::string& getHostname() const { return m_hostname; }
    const std::string& getAdapterMAC() const { return m_mainAdapterMAC; }
    const std::string& getAdapterIP() const { return m_mainAdapterIP; }
    bool isConnected() const;
    void setIsConnected(bool b);
private:
    bool shouldUpdate(uint32_t ticks) const override;

    void getDNSAndHostname();
    void getMACAndLocalIP();

    _MIB_IF_TABLE2* m_table{ nullptr };
    _MIB_IF_ROW2* m_adapterEntry{ nullptr };

    std::string m_DNSIP{ "0.0.0.0" };
    std::string m_hostname{ "" };
    std::string m_mainAdapterMAC{ "00-00-00-00-00-00" };
    std::string m_mainAdapterIP{ "0.0.0.0" };

    std::string m_pingServer{ "" };
    std::atomic<bool> m_isConnected{ false };
    std::atomic<bool> m_threadRunning{ false };
    std::thread m_netConnectionThread{ };
    uint32_t m_pingFreqMs{ 0U };

    uint64_t m_downMaxVal{ 10U * GB };
    uint64_t m_upMaxVal{ 10U * GB };
    size_t dataSize{ 40U };
    std::vector<uint64_t> m_downBytes{ };
    std::vector<uint64_t> m_upBytes{ };
};

} // namespace rg
