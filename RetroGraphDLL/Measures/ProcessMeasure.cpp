module RG.Measures:ProcessMeasure;

import :NtDefs;

import UserSettings;
import Utils;

import RG.Core;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

#pragma comment(lib, "Ntdll.lib")

namespace rg {

ProcessMeasure::ProcessMeasure()
    : m_numCPUProcessesToDisplay{ UserSettings::inst().getVal<int, unsigned int>("Widgets-ProcessesCPU.NumProcessesDisplayed") }
    , m_numRAMProcessesToDisplay{ UserSettings::inst().getVal<int, unsigned int>("Widgets-ProcessesRAM.NumProcessesDisplayed") }
    , m_configRefreshedHandle{
        UserSettings::inst().configRefreshed.attach(
            [&]() {
                m_numCPUProcessesToDisplay = UserSettings::inst().getVal<int, unsigned int>("Widgets-ProcessesCPU.NumProcessesDisplayed");
                m_numRAMProcessesToDisplay = UserSettings::inst().getVal<int, unsigned int>("Widgets-ProcessesRAM.NumProcessesDisplayed");
            })
    } {

    if constexpr (!debugMode) {
        // Set the debug privilege in order to gain access to system processes
        HANDLE hToken;
        RGVERIFY(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken), "Failed OpenThreadToken");

        if (!setDebugPrivileges(hToken, SE_DEBUG_NAME, true)) {
            CloseHandle(hToken);
            showMessageBox("Failed to set privilege, please run as administrator to get all process data");
        }
    }

    populateList();
    fillRAMData();
}

ProcessMeasure::~ProcessMeasure() {
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
}

bool ProcessMeasure::updateInternal() {
    // Update the process list vector every 10 seconds
    static Timer newProcessUpdateTimer{ std::chrono::seconds{ 10 } };
    if (newProcessUpdateTimer.hasElapsed()) {
        detectNewProcesses();
        fillRAMData();
        newProcessUpdateTimer.restart();
    }

    // Track iterator outside while scope for std::erase
    auto it{ m_allProcessData.begin() };
    while (it != m_allProcessData.end()) {
        auto& pd{ **it };

        // Get the process relating to the ProcessData object
        const auto pHandle{ OpenProcess(PROCESS_QUERY_INFORMATION |
                                        PROCESS_VM_READ, false,
                                        pd.getPID()) };
        if (!pHandle) {
            const auto error{ GetLastError() };
            // If access is denied or the process is the system idle 
            // process, just silently skip the process
            if (error != ERROR_ACCESS_DENIED && pd.getPID() != 0) {
                printf(std::format("Failed to open process. Code: {}. ProcessID: {}\n", error, pd.getPID()).c_str());
            }
            continue;
        }

        // Check for processes that have exited and remove them from the list
        auto exitCode = DWORD{ 1U };
        if (!GetExitCodeProcess(pHandle, &exitCode)) {
            printf(std::format("Failed to retreive exit code of process. Error: {}", GetLastError()).c_str());
            CloseHandle(pHandle);
            continue;
        }
        if (exitCode == 0 || exitCode == 1) {
            it = m_allProcessData.erase(it);
        } else {
            // Get new timing information and calculate the CPU usage
            const auto cpuUsage{ calculateCPUUsage(pHandle, pd) };
            pd.setCpuUsage(cpuUsage);
            pd.updateMemCounters();

            ++it;
        }
        CloseHandle(pHandle);
    }

    fillCPUData();

    return true;
}

int ProcessMeasure::getPIDFromName(std::string_view name) const {
    const auto it{ std::find_if(m_allProcessData.cbegin(),
                                m_allProcessData.cend(),
            [&name](const auto& sp) {
                return sp->getName() == name;
            }) 
    };

    if (it != m_allProcessData.cend()) {
        return (*it)->getPID();
    } else {
        return -1;
    }
}

bool ProcessMeasure::setDebugPrivileges(HANDLE hToken, LPCTSTR privilege,
                                        bool enablePrivilege) {
    LUID luid;
    TOKEN_PRIVILEGES tpPrevious;

    if(!LookupPrivilegeValue(nullptr, privilege, &luid)) 
        return false;

    // first pass.  get current privilege setting
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount           = 1;
    tp.Privileges[0].Luid       = luid;
    tp.Privileges[0].Attributes = 0;

    DWORD cbPrevious{ sizeof(TOKEN_PRIVILEGES) };
    AdjustTokenPrivileges(
        hToken, false, &tp, sizeof(TOKEN_PRIVILEGES), &tpPrevious, &cbPrevious
    );

    if (GetLastError() != ERROR_SUCCESS) 
        return false;

    // second pass.  set privilege based on previous setting
    tpPrevious.PrivilegeCount       = 1;
    tpPrevious.Privileges[0].Luid   = luid;

    if(enablePrivilege) {
        tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
    }
    else {
        tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
                                                tpPrevious.Privileges[0].Attributes);
    }

    AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, nullptr, nullptr);

    if (GetLastError() != ERROR_SUCCESS) 
        return false;

    return true;
}


void ProcessMeasure::fillCPUData() {
    // Sort based on the current CPU usage of processes in descending order
    std::sort(m_allProcessData.begin(), m_allProcessData.end(),
    [](const auto& ppd1, const auto& ppd2) {
        return ppd1->getCpuUsage() > ppd2->getCpuUsage();
    });

    // Update the strings to be drawn
    m_procCPUListData.clear();
    for (const auto& ppd : m_allProcessData) {
        m_procCPUListData.emplace_back(ppd->getName(), ppd->getCpuUsage());

        if (m_procCPUListData.size() >= m_numCPUProcessesToDisplay)
            break;
    }
}

void ProcessMeasure::fillRAMData() {
    // Now sort the list in terms of memory usage and build strings for that
    std::sort(m_allProcessData.begin(), m_allProcessData.end(),
        [](const auto& ppd1, const auto& ppd2) {
            return ppd1->getWorkingSetSizeMB() > ppd2->getWorkingSetSizeMB();
        });

    m_procRAMListData.clear();
    for (const auto& ppd : m_allProcessData) {
        m_procRAMListData.emplace_back(ppd->getName(), ppd->getWorkingSetSizeMB());

        if (m_procRAMListData.size() >= m_numRAMProcessesToDisplay)
            break;
    }
}

double ProcessMeasure::calculateCPUUsage(HANDLE pHandle, ProcessData& oldData) {
    // Find delta in the process's total CPU usage time
    FILETIME cTime;
    FILETIME eTime;
    FILETIME kTime;
    FILETIME uTime;
    GetProcessTimes(pHandle, &cTime, &eTime, &uTime, &kTime);
    const auto procKernelDiff{ subtractTimes(kTime, oldData.getKernelTime()) };
    const auto procUserDiff{ subtractTimes(uTime, oldData.getUserTime()) };
    const auto totalProc{ procKernelDiff + procUserDiff };

    // Find delta in the entire system's CPU usage time
    FILETIME sysIdle, sysKernel, sysUser;
    GetSystemTimes(&sysIdle, &sysKernel, &sysUser);
    const auto sysKernelDiff{ subtractTimes(sysKernel, oldData.getLastSystemKernelTime()) };
    const auto sysUserDiff{ subtractTimes(sysUser, oldData.getLastSystemUserTime()) };
    const auto totalSys{ sysKernelDiff + sysUserDiff };

    // Get the CPU usage as a percentage
    const double cpuUse = static_cast<double>(100 * totalProc) / static_cast<double>(totalSys);

    oldData.setTimes(cTime, eTime, kTime, uTime);

    return cpuUse;
}

void ProcessMeasure::populateList() {
    // Allocate buffer for the process list to fill
    // We need to allocate a large buffer because the process list can be large.
    PVOID buffer{ VirtualAlloc(nullptr, 1024*1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) }; 
    if (!buffer) {
        RGERROR(std::format("Error: Unable to allocate memory for process list: {}", GetLastError()).c_str());
        return;
    }

    PSYSTEM_PROCESS_INFO spi{ static_cast<PSYSTEM_PROCESS_INFO>(buffer) };

    // Fill the buffer with process information structs
    NTSTATUS status;
    if(!NT_SUCCESS(status = NtQuerySystemInformation(SystemProcessInformation, spi, 1024*1024, nullptr))) {
        VirtualFree(buffer, 0, MEM_RELEASE);
        RGERROR(std::format("Error: Unable to query process list: {}", status).c_str());
        return;
    }

    // Loop over the process list and fill allProcessData with new ProcessData
    // object for each process
    for ( ;
          spi->NextEntryOffset;
          spi = reinterpret_cast<PSYSTEM_PROCESS_INFO>(reinterpret_cast<LPBYTE>(spi) + spi->NextEntryOffset)) {

        const auto procID{ reinterpret_cast<LONGLONG>(spi->ProcessId) };

        const auto pHandle{ OpenProcess(PROCESS_QUERY_INFORMATION |
                                        PROCESS_VM_READ, false, static_cast<DWORD>(procID)) };
        if (!pHandle) {
            const auto error{ GetLastError() };
            // If access is denied or the process is the system idle process,
            // just silently skip the process
            if (error != ERROR_ACCESS_DENIED && procID != 0) {
                RGERROR(std::format("Failed to open process. Code: {}. ProcessID: {}", error, procID).c_str());
            }
        } else {
            // Convert the process name from wchar* to char*
            size_t charsConverted{ 0U };
            char* nameBuff = new char[spi->ImageName.Length];
            wcstombs_s(&charsConverted, nameBuff, spi->ImageName.Length, spi->ImageName.Buffer, spi->ImageName.Length);

            m_allProcessData.emplace_back(std::make_unique<ProcessData>(pHandle, static_cast<DWORD>(procID), nameBuff));

            delete[] nameBuff;
        }
    }

    VirtualFree(buffer, 0, MEM_RELEASE); // Free the allocated buffer.
}

void ProcessMeasure::detectNewProcesses() {
    // We need to allocate a large buffer because the process list can be large.
    PVOID buffer{ VirtualAlloc(nullptr, 1024*1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };
    if (!buffer) {
        RGERROR(std::format("Unable to allocate memory for process list: ", GetLastError()).c_str());
        return;
    }

    PSYSTEM_PROCESS_INFO spi{ static_cast<PSYSTEM_PROCESS_INFO>(buffer) };

    // Fill the buffer with process information structs
    NTSTATUS status;
    if(!NT_SUCCESS(status = NtQuerySystemInformation(SystemProcessInformation, spi, 1024*1024, nullptr))) {
        VirtualFree(buffer, 0, MEM_RELEASE);
        RGERROR(std::format("Error: Unable to query process list ", status).c_str());
        return;
    }

    // Loop over the process list for any new processes
    for ( ;
          spi->NextEntryOffset;
          spi = reinterpret_cast<PSYSTEM_PROCESS_INFO>(reinterpret_cast<LPBYTE>(spi) + spi->NextEntryOffset)) {

        const auto procID{ reinterpret_cast<LONGLONG>(spi->ProcessId) };

        const auto it{ std::find_if(m_allProcessData.cbegin(),
                                    m_allProcessData.cend(),
                                    [procID](const auto& ppd) {
                                        return procID == ppd->getPID();
                                    })};

        // If it doesn't exist, create a new ProcessData object in the list
        if (it == m_allProcessData.cend()) {
            auto pHandle{ OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, static_cast<DWORD>(procID)) };
            if (!pHandle) {
                const auto error{ GetLastError() };
                // If access is denied or the process is the system idle
                // process, just silently skip the process
                if (error != ERROR_ACCESS_DENIED && procID != 0) {
                    RGERROR(std::format("Failed to open process. Code: {}. ProcessID: {}", error, procID).c_str());
                }
            } else {
                // Convert the ImageName buffer from wchar* to char*
                auto charsConverted = size_t{ 0U };
                std::vector<char> nameBuff(spi->ImageName.Length);
                auto const errCode{ wcstombs_s(&charsConverted, nameBuff.data(), spi->ImageName.Length,
                                               spi->ImageName.Buffer, spi->ImageName.Length) };
                if (errCode) {
                    RGERROR(std::format("Failed to convert process name encoding: {}", errCode).c_str());
                } else {
                    m_allProcessData.emplace_back(
                        std::make_unique<ProcessData>(pHandle, static_cast<DWORD>(procID), nameBuff.data())
                    );
                }

            }
        }
    }

    VirtualFree(buffer, 0, MEM_RELEASE); // Free the allocated buffer.
}

} // namespace rg
