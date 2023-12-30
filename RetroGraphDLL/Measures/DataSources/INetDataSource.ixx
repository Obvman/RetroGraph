export module RG.Measures.DataSources:INetDataSource;

import std.core;

namespace rg {

export class INetDataSource {
public:
    virtual ~INetDataSource() = default;

    virtual void updateBestAdapter() = 0;
    virtual void updateNetTraffic() = 0;
    virtual bool checkConnectionStatusChanged() = 0;

    virtual int64_t getDownBytes() const = 0;
    virtual int64_t getUpBytes() const = 0;
    virtual const std::string& getDNS() const = 0;
    virtual const std::string& getHostname() const = 0;
    virtual const std::string& getAdapterMAC() const = 0;
    virtual const std::string& getAdapterIP() const = 0;
    virtual bool isConnected() const = 0;
};

} // namespace rg
