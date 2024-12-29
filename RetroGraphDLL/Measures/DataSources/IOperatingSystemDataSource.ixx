export module RG.Measures.DataSources:IOperatingSystemDataSource;

import std;

namespace rg {

using namespace std::chrono;

export struct OperatingSystemData {
    std::string osName;
    std::string osVersion;
    std::string userName;
    std::string computerName;

    auto operator<=>(const OperatingSystemData&) const = default;
};

export class IOperatingSystemDataSource {
public:
    virtual ~IOperatingSystemDataSource() = default;

    virtual OperatingSystemData getOperatingSystemData() const = 0;
};

} // namespace rg
