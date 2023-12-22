export module RG.Measures:RAMMeasure;

import :Measure;

import RG.Core;
import RG.Measures.DataSources;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

export using RAMUsageEvent = CallbackEvent<float>;

/* Stores capacity totals and availability for system RAM */
export class RAMMeasure : public Measure {
public:
    RAMMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<const IRAMDataSource> ramDataSource);
    ~RAMMeasure() noexcept = default;

    uint64_t getRAMCapacity() const { return m_ramDataSource->getRAMCapacity(); }

    RAMUsageEvent onRAMUsage;

protected:
    /* Updates the system memory status values */
    bool updateInternal() override;

private:
    std::unique_ptr<const IRAMDataSource> m_ramDataSource;
};

} // namespace rg
