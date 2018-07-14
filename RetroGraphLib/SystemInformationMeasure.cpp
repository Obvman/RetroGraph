#include "stdafx.h"
#include "SystemInformationMeasure.h"

#include "utils.h"

#include <winternl.h>
#pragma comment(lib, "Ntdll.lib")

#include "NtDefs.h"

namespace rg {

SystemInformationMeasure::SystemInformationMeasure() :
    Measure{ 2 } {
}

void SystemInformationMeasure::update(int) {
    //static LONGLONG prev;

    //SYSTEM_PERFORMANCE_INFORMATION spi;
    //NtQuerySystemInformation(SystemPerformanceInformation, &spi, sizeof(spi), nullptr);

    //auto curr = spi.IoWriteTransferCount.QuadPart;
    //auto delta = curr - prev;
    //std::cout << "prev: " << prev << '\n';
    //std::cout << "curr: " << curr << '\n';
    //std::cout << "delta: " << delta << '\n';

    //prev = curr;
}

bool SystemInformationMeasure::shouldUpdate(int ticks) const {
    return ticksMatchSeconds(ticks, m_updateRates.front());
}


}
