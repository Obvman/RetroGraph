#include "../headers/GraphData.h"

#include <iostream>

namespace rg {

GraphData::GraphData(size_t dataSize) :
    m_data{},
    m_startIndex{ 0U } {

    m_data.resize(dataSize);
}

GraphData::~GraphData() {
}

void GraphData::addValue(float v) {
    //std::cout << "Adding value to graph data: \n" << m_data.size() << '\n';

}

}