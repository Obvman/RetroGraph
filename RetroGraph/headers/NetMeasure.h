#pragma once

#include <string>
#include <vector>

struct _MIB_IF_ROW2;

namespace rg {

class NetMeasure {
public:
    NetMeasure();
    ~NetMeasure();

    void update();

    uint64_t getMaxDownValue() const { return m_downMaxVal; }
    uint64_t getMaxUpValue() const { return m_upMaxVal; }
    const std::vector<uint64_t>& getDownData() const { return m_downBytes; }
    const std::vector<uint64_t>& getUpData() const { return m_upBytes; }
private:

    // Just hard code my network adapter name
    const wchar_t* myAdapter{ L"Intel(R) Ethernet Connection (2) I219-V" };
    _MIB_IF_ROW2* m_adapterEntry;

    uint64_t m_downMaxVal;
    uint64_t m_upMaxVal;

    size_t dataSize;
    std::vector<uint64_t> m_downBytes;
    std::vector<uint64_t> m_upBytes;
};

}