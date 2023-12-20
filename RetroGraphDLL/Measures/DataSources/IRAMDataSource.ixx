export module RG.Measures.DataSources:IRAMDataSource;

import std.core;

namespace rg {

export class IRAMDataSource {
public:
    virtual ~IRAMDataSource() = default;

    // Returns the ram usage as a percentage between 0.0 - 1.0
    virtual float getRAMUsage() const = 0;

    // Returns the ram capacity in bytes
    virtual uint64_t getRAMCapacity() const = 0;
};

}
