export module RG.Measures:NetMeasure;

import :Measure;

import RG.Core;
import RG.Measures.DataSources;

import std.core;

namespace rg {

export using NetUsageEvent = CallbackEvent<int64_t>;
export using ConnectionStatusChangedEvent = CallbackEvent<bool>;
// #TODO event for when best adapter changes

export class NetMeasure : public Measure {
public:
    NetMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<INetDataSource> netDataSource);

    const std::string& getDNS() const { return m_netDataSource->getDNS(); }
    const std::string& getHostname() const { return m_netDataSource->getHostname(); }
    const std::string& getAdapterMAC() const { return m_netDataSource->getAdapterMAC(); }
    const std::string& getAdapterIP() const { return m_netDataSource->getAdapterIP(); }
    bool isConnected() const { return m_netDataSource->isConnected(); }

    NetUsageEvent onDownBytes;
    NetUsageEvent onUpBytes;
    ConnectionStatusChangedEvent onConnectionStatusChanged;

protected:
    bool updateInternal() override;

private:
    std::unique_ptr<INetDataSource> m_netDataSource;
};

} // namespace rg
