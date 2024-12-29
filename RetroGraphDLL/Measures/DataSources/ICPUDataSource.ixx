export module RG.Measures.DataSources:ICPUDataSource;

import std;

namespace rg {

export class ICPUDataSource {
public:
    virtual ~ICPUDataSource() = default;

    virtual void update() = 0;

    virtual const std::string& getCPUName() const = 0;
    virtual float getCPUUsage() const = 0;
    virtual int getNumCores() const = 0;
    virtual float getCoreUsage(int coreIdx) const = 0;
    virtual float getClockSpeed() const = 0;
    virtual float getVoltage() const = 0;
    virtual float getTemp(int coreNum) const = 0;
};

} // namespace rg
