#pragma once

#include <string>
#include <vector>

struct _MIB_IF_ROW2;

namespace rg {

class NetMeasure {
public:
    NetMeasure(const std::string& netAdapterName);
    ~NetMeasure();

    void update();

    uint64_t getMaxDownValue() const { return m_downMaxVal; }
    uint64_t getMaxUpValue() const { return m_upMaxVal; }
    const std::vector<uint64_t>& getDownData() const { return m_downBytes; }
    const std::vector<uint64_t>& getUpData() const { return m_upBytes; }
    const std::string& getDNS() const { return m_DNSIP; }
    const std::string& getHostname() const { return m_hostname; }
    const std::string& getAdapterMAC() const { return m_mainAdapterMAC; }
    const std::string& getAdapterIP() const { return m_mainAdapterIP; }
private:
    void getNetStats();

    _MIB_IF_ROW2* m_adapterEntry;
    std::string m_mainAdapter;

    std::string m_DNSIP;
    std::string m_hostname;
    std::string m_mainAdapterMAC;
    std::string m_mainAdapterIP;

    uint64_t m_downMaxVal;
    uint64_t m_upMaxVal;
    size_t dataSize;
    std::vector<uint64_t> m_downBytes;
    std::vector<uint64_t> m_upBytes;
};

}