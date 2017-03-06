#include <iostream>
#include <iomanip>
#include <chrono>

#include <Windows.h>
#include <winternl.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "Ntdll.lib")

#include <GL/glew.h>
#include <GL/gl.h>

#include "../headers/utils.h"
#include "../headers/CPUMeasure.h"
#include "../headers/Window.h"

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

    try {
        auto mainWindow = rg::Window{ hInstance, "RetroGraph",
                                      1920U, 1170U, 2560, 0 };
        mainWindow.init();

        mainLoop(mainWindow);
    } catch(const std::runtime_error& e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}

IWbemServices *pSvc = 0;
IWbemLocator *pLoc = 0;

int testWMITemp() {
    HRESULT hres;

    // Initialize COM.
    hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        std::cout << "Failed to initialize COM library. "
            << "Error code = 0x"
            << std::hex << hres << '\n';
        return 1;              // Program has failed.
    }

    // Initialize
    hres =  CoInitializeSecurity(
        NULL,
        -1,      // COM negotiates service
        NULL,    // Authentication services
        NULL,    // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
        RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
        NULL,             // Authentication info
        EOAC_NONE,        // Additional capabilities
        NULL              // Reserved
    );


    if (FAILED(hres))
    {
        std::cout << "Failed to initialize security. "
            << "Error code = 0x"
            << std::hex << hres << '\n';
        CoUninitialize();
        return 1;          // Program has failed.
    }

    // Obtain the initial locator to Windows Management
    // on a particular host computer.

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID *) &pLoc);

    if (FAILED(hres))
    {
        std::cout << "Failed to create IWbemLocator object. "
            << "Error code = 0x"
            << std::hex << hres << '\n';
        CoUninitialize();
        return 1;       // Program has failed.
    }

    // Connect to the root\cimv2 namespace with the
    // current user and obtain pointer pSvc
    // to make IWbemServices calls.

    hres = pLoc->ConnectServer(

        _bstr_t(L"ROOT\\CIMV2"), // WMI namespace
        NULL,                    // User name
        NULL,                    // User password
        0,                       // Locale
        NULL,                    // Security flags
        0,                       // Authority
        0,                       // Context object
        &pSvc                    // IWbemServices proxy
    );

    if (FAILED(hres))
    {
        std::cout << "Could not connect. Error code = 0x"
            << std::hex << hres << '\n';
        pLoc->Release();
        CoUninitialize();
        return 1;                // Program has failed.
    }

    std::cout << "Connected to ROOT\\CIMV2 WMI namespace" << '\n';

    // Set the IWbemServices proxy so that impersonation
    // of the user (client) occurs.
    hres = CoSetProxyBlanket(

        pSvc,                         // the proxy to set
        RPC_C_AUTHN_WINNT,            // authentication service
        RPC_C_AUTHZ_NONE,             // authorization service
        NULL,                         // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,       // authentication level
        RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
        NULL,                         // client identity
        EOAC_NONE                     // proxy capabilities
    );

    if (FAILED(hres)) {
        std::cout << "Could not set proxy blanket. Error code = 0x"
            << std::hex << hres << '\n';
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_NetworkAdapter"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cout << "Query for temperature failed. "
            << "Error code = 0x"
            << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;               // Program has failed.
    } else {
        IWbemClassObject *pclsObj;
        ULONG uReturn = 0;

        while (pEnumerator) {
            hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if(uReturn == 0) {
                break;
            }

            VARIANT vtProp;

            // Get the value of the Name property
            hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
            //std::wcout << "Adapter Name : " << vtProp.bstrVal << std::endl;
            VariantClear(&vtProp);
        }
    }

    //pSvc->Release();
    //pLoc->Release();
    //CoUninitialize();
    return 0;
}

void testWMIUpdate() {
    IEnumWbemClassObject* pEnumerator = NULL;
    HRESULT hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_NetworkAdapter"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres)) {
        std::cout << "Query for temperature failed. " << "Error code = 0x"
                  << std::hex << hres << std::endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;               // Program has failed.
    } else {
        IWbemClassObject *pclsObj;
        ULONG uReturn = 0;

        while (pEnumerator) {
            pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if(uReturn == 0) {
                break;
            }

            VARIANT vtProp;

            // Get the value of the Name property
            pclsObj->Get(L"NetEnabled", 0, &vtProp, 0, 0);
            if (vtProp.boolVal) {
                pclsObj->Get(L"Name", 0, &vtProp, 0, 0);

                std::wcout << "Adapter Name : " << vtProp.bstrVal << std::endl;

                // We only want to monitor at most one adapter, so break when
                // once we've found it
                break;
            }

            VariantClear(&vtProp);
        }
    }
}

void mainLoop(rg::Window& mainWindow) {
    constexpr auto framesPerSecond = uint32_t{ 2U };

    using namespace std::chrono;

    auto start{ duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() };
    auto ticks = uint32_t{ 1 };
    auto lastTick{ ticks };
    MSG msg;

    //testWMITemp();

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

            if ((ticks % (rg::ticksPerSecond * 1)) == 0) {
                //testWMIUpdate();
            }

            // Draw according to the framerate
            if ((lastTick % std::lround(static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {
                mainWindow.draw(ticks);
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

