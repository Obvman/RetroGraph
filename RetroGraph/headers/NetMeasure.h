#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

#include "UserSettings.h"
#include "Measure.h"

struct _MIB_IF_ROW2;
struct _MIB_IF_TABLE2;

namespace rg {

class NetMeasure : public Measure {
public:
    NetMeasure(const UserSettings& settings);
    virtual ~NetMeasure();
    NetMeasure(const NetMeasure&) = delete;
    NetMeasure& operator=(const NetMeasure&) = delete;

    virtual void init() override;
    virtual void update(uint32_t ticks) override;

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
    void getDNSAndHostname();
    void getMACAndLocalIP();

    _MIB_IF_TABLE2* m_table;
    _MIB_IF_ROW2* m_adapterEntry;
    std::string m_mainAdapter;

    std::string m_DNSIP;
    std::string m_hostname;
    std::string m_mainAdapterMAC;
    std::string m_mainAdapterIP;

    std::string m_pingServer;
    std::atomic<bool> m_isConnected;
    std::thread m_netConnectionThread;
    uint32_t m_pingFreqMs;

    uint64_t m_downMaxVal;
    uint64_t m_upMaxVal;
    size_t dataSize;
    std::vector<uint64_t> m_downBytes;
    std::vector<uint64_t> m_upBytes;
};

}
