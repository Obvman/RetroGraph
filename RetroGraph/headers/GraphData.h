#pragma once

#include <vector>
#include <stdint.h>

namespace rg {

class GraphData {
public:
    GraphData(size_t dataSize);
    ~GraphData() = default;
    GraphData(const GraphData&) = delete;
    GraphData& operator=(const GraphData&) = delete;

    void addValue(float v);
private:
    std::vector<float> m_data;
    size_t m_startIndex;
};

}
