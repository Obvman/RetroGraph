export module RG.Measures.DataSources:Win32NetDataSource;

import :INetDataSource;
import :NetworkConnectionChecker;

import std;

import "WindowsNetworkHeaderUnit.h";

namespace rg {

export class Win32NetDataSource : public INetDataSource {
public:
    Win32NetDataSource(std::chrono::milliseconds pingFrequency, const std::string& pingServer);

    bool updateBestAdapter() override;
    void updateNetTraffic() override;
    bool checkConnectionStatusChanged() override { return m_connectionChecker.checkConnectionStatusChanged(); }

    int64_t getDownBytes() const override { return m_downBytes; }
    int64_t getUpBytes() const override { return m_upBytes; }

    const std::string& getDNS() const override { return m_dnsIP; }
    const std::string& getHostname() const override { return m_hostname; }
    const std::string& getAdapterName() const override { return m_mainAdapterName; }
    const std::string& getAdapterMAC() const override { return m_mainAdapterMAC; }
    const std::string& getAdapterIP() const override { return m_mainAdapterIP; }
    bool isConnected() const override { return m_connectionChecker.isConnected(); }

private:
    _MIB_IF_TABLE2* getIfTable() const;
    int getBestAdapterIndex() const;
    _MIB_IF_ROW2* getBestAdapterRow(int bestIfaceIndex) const;
    IP_ADAPTER_INFO determineBestAdapter() const;
    std::string determineMAC(const IP_ADAPTER_INFO& adapter) const;
    std::string determineDNSIP() const;
    std::string determineHostname() const;
    void getFixedInfoBuffer(std::vector<std::byte>& fixedInfoBuffer) const;

    _MIB_IF_TABLE2* m_table;
    int m_bestIfaceIndex;
    _MIB_IF_ROW2* m_adapterRow;
    IP_ADAPTER_INFO m_bestAdapter;

    int64_t m_downBytes;
    int64_t m_upBytes;
    std::string m_dnsIP;
    std::string m_hostname;
    std::string m_mainAdapterName;
    std::string m_mainAdapterMAC;
    std::string m_mainAdapterIP;
    NetworkConnectionChecker m_connectionChecker;
};

} // namespace rg
