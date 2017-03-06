#pragma once

#include <string>

namespace rg {

class NetMeasure {
public:
    NetMeasure();
    ~NetMeasure();

    void update();
private:

    std::string m_MAC;
};

}