#pragma once

#include <vector>

namespace rg {

class CPUMeasure {
public:
    CPUMeasure();
    ~CPUMeasure();

    float getCPULoad();

    std::vector<float> m_usageData;
private:
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);


    size_t dataSize; // max number of usage percentages to store
};

}