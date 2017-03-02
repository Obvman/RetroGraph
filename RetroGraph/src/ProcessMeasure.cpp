#include "../headers/ProcessMeasure.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>

#include <winternl.h>
#pragma comment(lib, "Ntdll.lib")

#include <GL/freeglut.h>
#include <GL/gl.h>

#include "../headers/colors.h"
#include "../headers/ProcessData.h"
#include "../headers/utils.h"

namespace rg {

typedef struct _SYSTEM_PROCESS_INFO {
    ULONG                   NextEntryOffset;
    ULONG                   NumberOfThreads;
    LARGE_INTEGER           Reserved[3];
    LARGE_INTEGER           CreateTime;
    LARGE_INTEGER           UserTime;
    LARGE_INTEGER           KernelTime;
    UNICODE_STRING          ImageName;
    ULONG                   BasePriority;
    HANDLE                  ProcessId;
    HANDLE                  InheritedFromProcessId;
} SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;

ProcessMeasure::ProcessMeasure() :
    m_allProcessData{},
    m_numProcessesToDisplay{ 7 },
    m_procCPUListData{ m_numProcessesToDisplay },
    m_procRAMListData{ m_numProcessesToDisplay } { }

ProcessMeasure::~ProcessMeasure() {
}

void ProcessMeasure::init() {
    populateList();
    fillRAMData();
}
// TODO benchmark using vector vs list

void ProcessMeasure::update(uint32_t ticks) {
    // Update the process list vector every 10 seconds
    if ((ticks % (ticksPerSecond * 10)) == 0) {
        detectNewProcesses();
    }

    if ((ticks % (ticksPerSecond * 2)) == 0) {
        // Track iterator outside while scope for std::erase
        auto it = m_allProcessData.begin();
        while (it != m_allProcessData.end()) {
            auto& pd = **it; // get reference to ProcessData for convenience

            // Get the process relating to the ProcessData object
            const auto pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                         false, pd.getPID());
            if (!pHandle) {
                auto error = GetLastError();
                // If access is denied or the process is the system idle process, just silently skip the process
                if (error != ERROR_ACCESS_DENIED && pd.getPID() != 0) {
                    fatalMessageBox("Failed to open process. Code: " + std::to_string(error));
                }
                continue;
            }

            // Check for processes that have exited and remove them from the list
            auto exitCode = DWORD{ 1U };
            GetExitCodeProcess(pHandle, &exitCode);
            if (exitCode == 0 || exitCode == 1) {
                it = m_allProcessData.erase(it);
            } else {
                // Get new timing information and calculate the CPU usage
                const auto cpuUsage{ calculateCPUUsage(pHandle, pd) };
                pd.setCpuUsage(cpuUsage);
                pd.updateMemCounters();

                ++it;
            }
        }

        fillCPUData();
    }

    if ((ticks % (ticksPerSecond * 5)) == 0) {
        fillRAMData();
    }
}


void ProcessMeasure::fillCPUData() {
    // Sort the vector based on the current CPU usage of processes in descending order
    std::sort(m_allProcessData.begin(), m_allProcessData.end(),
    [](const auto& ppd1, const auto& ppd2) {
        return ppd1->getCpuUsage() > ppd2->getCpuUsage();
    });

    // Update the strings to be drawn
    m_procCPUListData.clear();
    for (const auto& ppd : m_allProcessData) {

        m_procCPUListData.emplace_back(ppd->getName(), ppd->getCpuUsage());

        if (m_procCPUListData.size() >= m_numProcessesToDisplay) {
            break;
        }
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

        if (m_procRAMListData.size() >= m_numProcessesToDisplay) {
            break;
        }
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

        /*SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION sppi;
        ULONG len;
        NtQuerySystemInformation(SystemProcessorPerformanceInformation, &sppi, sizeof(sppi), &len);
        auto sysTime = sppi.KernelTime.QuadPart + sppi.UserTime.QuadPart;
        auto totalSys2 = sysTime - oldData.getLastSystemTime();
        const double cpuUse2 = static_cast<double>(100 * totalProc) / static_cast<double>(totalSys2);*/

        // Get the CPU usage as a percentage
        const double cpuUse = static_cast<double>(100 * totalProc) / static_cast<double>(totalSys);

        oldData.setTimes(cTime, eTime, kTime, uTime);

        return cpuUse;
}

void ProcessMeasure::populateList() {
    // Allocate buffer for the process list to fill
    NTSTATUS status;
    PVOID buffer;
    PSYSTEM_PROCESS_INFO spi;
    buffer = VirtualAlloc(NULL,1024*1024,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE); // We need to allocate a large buffer because the process list can be large.

    if(!buffer) {
        fatalMessageBox("Error: Unable to allocate memory for process list " +
                        std::to_string(GetLastError()) + '\n');
    }

    spi = static_cast<PSYSTEM_PROCESS_INFO>(buffer);

    // Fill the buffer with process information structs
    if(!NT_SUCCESS(status=NtQuerySystemInformation(SystemProcessInformation,spi,1024*1024,NULL))) {
        VirtualFree(buffer,0,MEM_RELEASE);
        fatalMessageBox("Error: Unable to query process list: " +
                        std::to_string(status) + '\n');
    }

    // Loop over the process list and fill allProcessData with new ProcessData
    // object for each process
    for ( ;
          spi->NextEntryOffset;
          spi = reinterpret_cast<PSYSTEM_PROCESS_INFO>(reinterpret_cast<LPBYTE>(spi) + spi->NextEntryOffset)) {

        const auto procID{ reinterpret_cast<DWORD>(spi->ProcessId) };

        const auto pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                         false, procID);
        if (!pHandle) {
            const auto error = GetLastError();
            // If access is denied or the process is the system idle process, just silently skip the process
            if (error != ERROR_ACCESS_DENIED && procID != 0) {
                fatalMessageBox("Failed to open process. Code: " + std::to_string(error));
            }
        } else {
            // Convert the process name from wchar* to char*
            size_t charsConverted{ 0U };
            char* nameBuff = new char[spi->ImageName.Length];
            wcstombs_s(&charsConverted, nameBuff, spi->ImageName.Length, spi->ImageName.Buffer, spi->ImageName.Length);

            m_allProcessData.emplace_back(std::make_shared<ProcessData>(pHandle,
                procID, nameBuff));

            delete[] nameBuff;
        }
    }

    VirtualFree(buffer,0,MEM_RELEASE); // Free the allocated buffer.
}

void ProcessMeasure::detectNewProcesses() {
    // Allocate buffer for the process list to fill
    NTSTATUS status;
    PVOID buffer;
    PSYSTEM_PROCESS_INFO spi;
    buffer=VirtualAlloc(NULL,1024*1024,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE); // We need to allocate a large buffer because the process list can be large.

    if(!buffer) {
        std::cout << "Error: Unable to allocate memory for process list " << GetLastError() << '\n';
        return;
    }

    spi = static_cast<PSYSTEM_PROCESS_INFO>(buffer);

    // Fill the buffer with process information structs
    if(!NT_SUCCESS(status=NtQuerySystemInformation(SystemProcessInformation,spi,1024*1024,NULL))) {
        std::cout << "Error: Unable to query process list " << status << '\n';
        VirtualFree(buffer,0,MEM_RELEASE);
        return;
    }

    // Loop over the process list for any new processes
    for ( ;
          spi->NextEntryOffset;
          spi = reinterpret_cast<PSYSTEM_PROCESS_INFO>(reinterpret_cast<LPBYTE>(spi) + spi->NextEntryOffset)) {

        const auto procID{ reinterpret_cast<DWORD>(spi->ProcessId) };

        const auto it{ std::find_if(m_allProcessData.cbegin(),
                                    m_allProcessData.cend(),
            [procID](const auto& ppd) {
                return procID == ppd->getPID();
        })};

        // If it doesn't exist, create a new ProcessData object in the list
        if (it == m_allProcessData.cend()) {
            auto pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                       false, procID);
            if (!pHandle) {
                const auto error = GetLastError();
                // If access is denied or the process is the system idle process, just silently skip the process
                if (error != ERROR_ACCESS_DENIED &&
                    procID != 0) {

                    fatalMessageBox("Failed to open process. Code: " + std::to_string(error));
                }
            } else {
                // Convert the ImageName buffer from wchar* to char*
                size_t charsConverted{ 0U };
                char* nameBuff = new char[spi->ImageName.Length];
                wcstombs_s(&charsConverted, nameBuff, spi->ImageName.Length, spi->ImageName.Buffer, spi->ImageName.Length);

                m_allProcessData.emplace_back(std::make_shared<ProcessData>(pHandle,
                    procID, nameBuff));

                delete[] nameBuff;
            }
        }
    }

    VirtualFree(buffer,0,MEM_RELEASE); // Free the allocated buffer.
}

}