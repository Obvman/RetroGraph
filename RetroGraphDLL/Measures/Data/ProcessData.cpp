module RG.Measures.Data:ProcessData;

import "RGAssert.h";

import "WindowsHeaderUnit.h";

namespace rg {

ProcessData::ProcessData(HANDLE pHandle, DWORD pID, const char* name)
    : m_pHandle{ pHandle }
    , m_processID{ pID }
    , m_procName{ name } {
    // Remove the ".exe" extension from the process name
    const auto p{ m_procName.find(".exe") };
    if (p != std::string::npos) {
        m_procName.erase(p, 4);
    }

    // If the name is too long, truncate and add ellipses
    constexpr size_t cutoffLen{ 26U };
    if (m_procName.length() >= cutoffLen) {
        m_procName.erase(cutoffLen, m_procName.length() - cutoffLen);
        m_procName.append("...");
    }

    FILETIME sysIdle; // Dummy ft, won't be used
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);

    // Get memory information for the process
    updateMemCounters();

    // Get CPU time information for the process
    RGVERIFY(GetProcessTimes(m_pHandle, &m_creationTime, &m_exitTime, &m_kernelTime, &m_userTime),
             "Failed to get process times.");
}

ProcessData::~ProcessData() {
    CloseHandle(m_pHandle);
}

void ProcessData::setTimes(const FILETIME& cTime, const FILETIME& eTime, const FILETIME& kTime, const FILETIME& uTime) {
    m_creationTime = cTime;
    m_exitTime = eTime;
    m_kernelTime = kTime;
    m_userTime = uTime;

    FILETIME sysIdle; // dummy
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);
}

void ProcessData::updateMemCounters() {
    RGVERIFY(GetProcessMemoryInfo(m_pHandle, &m_memCounters, sizeof(m_memCounters)),
             std::format("Failed to get process memory information. Error: ", GetLastError()).c_str());
}

} // namespace rg
