#include <iostream> // std
#include <iomanip>
#include <chrono>

#include <Windows.h> // win32
#include <winternl.h>
#include <Wbemidl.h>
#include <comdef.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "ntdll.lib")

#include <GL/glew.h> // openGL
#include <GL/gl.h>

#include "CPUMeasure.h"
#include "Window.h"

void test();
void test2();
ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);

//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    rg::Window mainWindow{ hInstance, "RetroGraph", 1920U, 1170U, 2560, 0 };

    using namespace std::chrono;
    auto start = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    constexpr uint32_t maxTicks{ 10000U };
    constexpr uint32_t tickDuration{ 100U }; // tick length in milliseconds
    uint32_t ticks{ 1 };
    auto lastTick = ticks;
    MSG msg;

    // Perform an update/draw call before entering the loop so the window shows
    // immediately at startup instead of after a few ticks
    mainWindow.init();

    //test2();
    test();

    // Enter main update/draw loop
    while(true) {
        const auto currTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        auto dt = currTime - start;

        // Handle windows messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Execute timed actions when the tick rolls over
        if (lastTick != ticks) {
            mainWindow.update(ticks);

            // Draw every 5 ticks
            if (lastTick % 5 == 0) {
                mainWindow.draw();
            }

            if (lastTick % 10 == 0) {
                //test2();
            }

            lastTick = ticks;
        }

        // Reset the counter every tick
        if (dt > tickDuration) {
            start = currTime;
            ++ticks;
        }

        // Keep the ticks counter range 1 - maxTicks to prevent overflow
        if (ticks > maxTicks) {
            ticks = 1U;
        }

        // Lay off the CPU a little
        Sleep(15);
    }
    return 0;
}

void test2() {
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres))
    {
        std::cout << "Failed to initialize COM library. Error code = 0x" 
            << std::hex << hres << std::endl;
        return;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );


    if (FAILED(hres))
    {
        std::cout << "Failed to initialize security. Error code = 0x" 
            << std::hex << hres << std::endl;
        CoUninitialize();
        return;                    // Program has failed.
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *) &pLoc);

    if (FAILED(hres))
    {
        std::cout << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << std::hex << hres << std::endl;
        CoUninitialize();
        return;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        NULL,                    // Security flags.
        0,                       // Authority (for example, Kerberos)
        0,                       // Context object 
        &pSvc                    // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        std::cout << "Could not connect. Error code = 0x" 
            << std::hex << hres << std::endl;
        pLoc->Release();     
        CoUninitialize();
        return;                // Program has failed.
    }

    std::cout << "Connected to ROOT\\CIMV2 WMI namespace" << std::endl;


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        std::cout << "Could not set proxy blanket. Error code = 0x" 
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
        return;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_PerfRawData_PerfProc_Process"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        std::cout << "Query for operating system name failed."
            << " Error code = 0x" 
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
                                       &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Name property

        static uint64_t oldClocks{ 0U };

        hr = pclsObj->Get(L"PercentProcessorTime", 0, &vtProp, 0, 0);
        auto newClocks{ vtProp.ullVal };

        //std::cout << vtProp.ullVal << ":" << vtProp.bstrVal << '\n';
        auto delta = newClocks - oldClocks;
        if (delta != 0) {
            VariantClear(&vtProp);
            hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);

            std::wcout << vtProp.bstrVal << ":\n";
            //std::wcout << delta << '\n';
        }

        oldClocks = newClocks;

        VariantClear(&vtProp);

        pclsObj->Release();
    }

    // Cleanup
    // ========

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    return;
}
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

    /*while(spi->NextEntryOffset) // Loop over the list until we reach the last entry.
    {
        printf("\nProcess name: %ws | Process ID: %d\n",spi->ImageName.Buffer,spi->ProcessId); // Display process information.
        spi=(PSYSTEM_PROCESS_INFO)((LPBYTE)spi+spi->NextEntryOffset); // Calculate the address of the next entry.
    }*/

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

        printf("%ws : %ws:\n", spi->ImageName.Buffer, spi2->ImageName.Buffer);
        auto cpuTime1 = spi->KernelTime.QuadPart + spi->UserTime.QuadPart;
        auto cpuTime2 = spi2->KernelTime.QuadPart + spi2->UserTime.QuadPart;

        auto diffProc = cpuTime2 - cpuTime1;

        auto sysKDiff = SubtractTimes(sysKernel2, sysKernel1);
        auto sysUDiff = SubtractTimes(sysUser2, sysUser1);
        auto diffSys = sysKDiff + sysUDiff;

        ULONGLONG cpuUse = (100.0 * diffProc) / diffSys;
        std::cout << cpuUse << "%\n";

        spi2=(PSYSTEM_PROCESS_INFO)((LPBYTE)spi2+spi2->NextEntryOffset); // Calculate the address of the next entry.
        spi=(PSYSTEM_PROCESS_INFO)((LPBYTE)spi+spi->NextEntryOffset); // Calculate the address of the next entry.
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
}