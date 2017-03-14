#pragma once

#include <vector>
#include <stdint.h>

namespace rg {

class GraphData {
public:
    GraphData(size_t dataSize);
    ~GraphData();

    void addValue(float v);
private:
    std::vector<float> m_data;
    size_t m_startIndex;
};

}