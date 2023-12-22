export module RG.Measures.DataSources:Win32RAMDataSource;

import :IRAMDataSource;

import "WindowsHeaderUnit.h";

namespace rg {

export class Win32RAMDataSource : public IRAMDataSource {
public:
    float getRAMUsage() const override;

    uint64_t getRAMCapacity() const override;

private:
    MEMORYSTATUSEX getMemoryStatus() const;
};

} // namespace rg
