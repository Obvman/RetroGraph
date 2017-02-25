#include <iostream> // std
#include <iomanip>
#include <chrono>

#include <Windows.h> // win32
#include <winternl.h>
#include <Wbemidl.h>
#include <comdef.h>

#include <GL/glew.h> // openGL
#include <GL/gl.h>

#include "utils.h"
#include "CPUMeasure.h"
#include "Window.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "ntdll.lib")

//void test();
//void test2();
void mainLoop(rg::Window& mainWindow);

#if (!_DEBUG)
// Release mode is a Win32 application, while Debug mode is a console application
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInstance = GetModuleHandle(nullptr);
#endif

#if _DEBUG
int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
#endif

    rg::Window mainWindow{ hInstance, "RetroGraph", 1920U, 1170U, 2560, 0 };

    // Perform an update/draw call before entering the loop so the window shows
    // immediately at startup instead of after a few ticks
    mainWindow.init();

    mainLoop(mainWindow);

    return 0;
}

void mainLoop(rg::Window& mainWindow) {
    using namespace std::chrono;
    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    uint32_t ticks{ 1 };
    auto lastTick = ticks;
    MSG msg;

    // Enter main update/draw loop
    while(true) {
        const auto currTime{ duration_cast<milliseconds>(
                                  system_clock::now().time_since_epoch()
                              ).count() };
        const auto dt{ currTime - start };

        // Handle windows messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Execute timed actions when the tick rolls over
        if (lastTick != ticks) {
            mainWindow.update(ticks);

            // Draw every half second
            if ((lastTick % (rg::ticksPerSecond/2)) == 0) {
                mainWindow.draw();
            }

            lastTick = ticks;
        }

        // Reset the millisecond counter every tick
        if (dt > rg::tickDuration) {
            start = currTime;
            ++ticks;
        }

        // Keep the ticks counter range 1 - maxTicks to prevent overflow
        if (ticks > rg::maxTicks) {
            ticks = 1U;
        }

        // Lay off the CPU a little
        Sleep(15);
    }
}

/*
typedef struct _SYSTEM_PROCESS_INFO
{
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
}SYSTEM_PROCESS_INFO,*PSYSTEM_PROCESS_INFO;

ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);

void test() {
    NTSTATUS status;
    PVOID buffer;
    PSYSTEM_PROCESS_INFO spi;

    buffer=VirtualAlloc(NULL,1024*1024,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE); // We need to allocate a large buffer because the process list can be large.

    if(!buffer)
    {
        std::cout << "Error: Unable to allocate memory for process list " << GetLastError() << '\n';
        return;
    }

    spi=(PSYSTEM_PROCESS_INFO)buffer;

    if(!NT_SUCCESS(status=NtQuerySystemInformation(SystemProcessInformation,spi,1024*1024,NULL)))
    {
        std::cout << "Error: Unable to query process list " << status << '\n';

        VirtualFree(buffer,0,MEM_RELEASE);
        return;
    }

    //while(spi->NextEntryOffset) // Loop over the list until we reach the last entry.  {
        //printf("\nProcess name: %ws | Process ID: %d\n",spi->ImageName.Buffer,spi->ProcessId); // Display process information.
        //spi=(PSYSTEM_PROCESS_INFO)((LPBYTE)spi+spi->NextEntryOffset); // Calculate the address of the next entry.
    //}

    FILETIME sysIdle1, sysKernel1, sysUser1;
    GetSystemTimes(&sysIdle1, &sysKernel1, &sysUser1);

    Sleep(1000);
    NTSTATUS status2;
    PVOID buffer2;
    PSYSTEM_PROCESS_INFO spi2;

    buffer2=VirtualAlloc(NULL,1024*1024,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE); // We need to allocate a large buffer because the process list can be large.

    if(!buffer2)
    {
        std::cout << "Error: Unable to allocate memory for process list " << GetLastError() << '\n';
        return;
    }

    spi2=(PSYSTEM_PROCESS_INFO)buffer2;

    if(!NT_SUCCESS(status2=NtQuerySystemInformation(SystemProcessInformation,spi2,1024*1024,NULL)))
    {
        std::cout << "Error: Unable to query process list " << status2 << '\n';

        VirtualFree(buffer2,0,MEM_RELEASE);
        return;
    }

    FILETIME sysIdle2, sysKernel2, sysUser2;
    GetSystemTimes(&sysIdle2, &sysKernel2, &sysUser2);

    // loop over both lists at the same time
    while(spi2->NextEntryOffset && spi->NextEntryOffset) // Loop over the list until we reach the last entry.
    {
        //printf("\nProcess name: %ws | Process ID: %d\n",spi2->ImageName.Buffer,spi2->ProcessId); // Display process information.

        //printf("%ws : %ws:\n", spi->ImageName.Buffer, spi2->ImageName.Buffer);
        const auto cpuTime1 = spi->KernelTime.QuadPart + spi->UserTime.QuadPart;
        const auto cpuTime2 = spi2->KernelTime.QuadPart + spi2->UserTime.QuadPart;

        const auto diffProc = cpuTime2 - cpuTime1;

        const auto sysKDiff = SubtractTimes(sysKernel2, sysKernel1);
        const auto sysUDiff = SubtractTimes(sysUser2, sysUser1);
        const auto diffSys = sysKDiff + sysUDiff;

        double cpuUse = static_cast<double>(100 * diffProc) / static_cast<double>(diffSys);
        //std::cout << cpuUse << "%\n";
        //std::cout << diffProc << '\n';

        spi2=(PSYSTEM_PROCESS_INFO)((LPBYTE)spi2+spi2->NextEntryOffset); // Calculate the address of the next entry.
        spi=(PSYSTEM_PROCESS_INFO)((LPBYTE)spi+spi->NextEntryOffset);
    }

    // Get the process list again

    VirtualFree(buffer2,0,MEM_RELEASE); // Free the allocated buffer.
    VirtualFree(buffer,0,MEM_RELEASE); // Free the allocated buffer.
}

ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB) {
     LARGE_INTEGER a, b;
     a.LowPart = ftA.dwLowDateTime;
     a.HighPart = ftA.dwHighDateTime;

     b.LowPart = ftB.dwLowDateTime;
     b.HighPart = ftB.dwHighDateTime;

     return a.QuadPart - b.QuadPart;
}*/