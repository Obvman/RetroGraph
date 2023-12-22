export module RG.Measures.DataSources:Win32OperatingSystemDataSource;

import :IOperatingSystemDataSource;

namespace rg {

export class Win32OperatingSystemDataSource : public IOperatingSystemDataSource {
public:
    OperatingSystemData getOperatingSystemData() const override;

private:
    std::string getOSVersion() const;
};

} // namespace rg
