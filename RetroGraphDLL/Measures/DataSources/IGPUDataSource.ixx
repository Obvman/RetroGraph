export module RG.Measures.DataSources:IGPUDataSource;

import std;

namespace rg {

export class IGPUDataSource {
public:
    virtual ~IGPUDataSource() = default;

    virtual const std::string& getGPUName() const = 0;
    virtual const std::string& getDriverVersion() const = 0;
    virtual float getGPUUsage() const = 0;
    virtual int getGPUTemp() const = 0;
    virtual int getGPUTotalMemoryKB() const = 0;
    virtual int getGPUAvailableMemoryKB() const = 0;
    virtual int getGPUFrameBufferSizeKB() const = 0;
    virtual int getGPUCoreCount() const = 0;
};

} // namespace rg
