#include "../headers/NetMeasure.h"

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x601

#include <iostream>
#include <Windows.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <Iphlpapi.h>

#include "../headers/utils.h"

#pragma comment(lib, "Iphlpapi.lib")

namespace rg {

/*std::string wstrToStr(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf16<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

std::wstring strToWstr(const std::string& str) {
    std::wstring_convert<std::codecvt_utf16<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}*/

NetMeasure::NetMeasure() :
    m_MAC{ "0000000000" }
    /*m_mainAdapterID{ L"-1" },
    mp_services{ nullptr },
    mp_locator{ nullptr }*/ {

    MIB_IF_TABLE2* table{ nullptr };
    if (GetIfTable2(&table) != NO_ERROR) {

    }
}

NetMeasure::~NetMeasure() {
    //releaseWMI();
}

void NetMeasure::update() {

}

/*void NetMeasure::initWMI() {
    // Initialize COM.
    auto hres{ CoInitializeEx(0, COINIT_MULTITHREADED) };
    if (FAILED(hres)) {
        fatalMessageBox("Failed to initialize COM library");
    }

    hres = CoInitializeSecurity(
        nullptr,
        -1,      // COM negotiates service
        nullptr,    // Authentication services
        nullptr,    // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
        RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
        nullptr,             // Authentication info
        EOAC_NONE,        // Additional capabilities
        nullptr              // Reserved
    );


    if (FAILED(hres)) {
        CoUninitialize();
        fatalMessageBox("Failed to initialize COM security");
    }

    // Obtain the initial locator to Windows Management
    // on a particular host computer.
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator, (LPVOID *) &mp_locator);

    if (FAILED(hres)) {
        CoUninitialize();
        fatalMessageBox("failed to create IWbemLocator object");
    }

    // Connect to the root\cimv2 namespace with the
    // current user and obtain pointer pSvc
    // to make IWbemServices calls.
    mp_locator->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // WMI namespace
        nullptr,                    // User name
        nullptr,                    // User password
        0,                       // Locale
        0,                    // Security flags
        0,                       // Authority
        0,                       // Context object
        &mp_services                    // IWbemServices proxy
    );

    if (FAILED(hres)) {
        mp_locator->Release();
        CoUninitialize();
        fatalMessageBox("Could not connect to WMI server");
    }

    // Set the IWbemServices proxy so that impersonation
    // of the user (client) occurs.
    hres = CoSetProxyBlanket(
        mp_services,                         // the proxy to set
        RPC_C_AUTHN_WINNT,            // authentication service
        RPC_C_AUTHZ_NONE,             // authorization service
        nullptr,                         // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,       // authentication level
        RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
        nullptr,                         // client identity
        EOAC_NONE                     // proxy capabilities
    );

    if (FAILED(hres)) {
        mp_services->Release();
        mp_locator->Release();
        CoUninitialize();
        fatalMessageBox("Could not set COM proxy blanket");
    }

    IEnumWbemClassObject* pEnumerator{ nullptr };
    hres = mp_services->ExecQuery(bstr_t("WQL"),
        bstr_t("SELECT NetEnabled, DeviceID FROM Win32_NetworkAdapter"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator);

    if (FAILED(hres)) {
        mp_services->Release();
        mp_locator->Release();
        CoUninitialize();
        fatalMessageBox("Initial WMI query failed");
    }

    // Iterate over the system's network adapters and store ID of the active
    // adapters
    IWbemClassObject *pclsObj;
    ULONG uReturn{ 0U };
    while (pEnumerator) {
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if(uReturn == 0) {
            break;
        }

        VARIANT vtProp;

        // Find the first connected network adapter and store the ID for future
        // queries
        pclsObj->Get(L"NetEnabled", 0, &vtProp, 0, 0);
        if (vtProp.boolVal) {
            if (pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0) != WBEM_S_NO_ERROR) {
                std::cout << "DeviceID query failed\n";
            }
            m_mainAdapterID = std::wstring{ vtProp.bstrVal };

            // Get static network information from the adapter
            if (pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0) == WBEM_E_NOT_FOUND) {
                std::wcout << L"Max speed: " << vtProp.bstrVal << "mbps\n";
            } else {
                std::cout << "Failed query\n";
            }
            //m_MAC = wstrToStr(std::wstring{ vtProp.bstrVal });
            break;
        }
        VariantClear(&vtProp);
    }

    // Check that the adapter string was initialised
    if (m_mainAdapterID == L"-1") {
        fatalMessageBox("Failed to get active network adapter from WMI");
    }

    std::cout << "MAC: " << m_MAC << '\n';
}

void NetMeasure::updateWMI() {
    const auto queryStr = std::wstring{
        L"SELECT * FROM Win32_NetworkAdapter WHERE DeviceID = " +
        m_mainAdapterID};

    IEnumWbemClassObject* pEnumerator{ nullptr };
    auto hres{ mp_services->ExecQuery(bstr_t{ "WQL" },
        bstr_t{ queryStr.c_str() },
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator) };

    if (FAILED(hres)) {
        mp_services->Release();
        mp_locator->Release();
        CoUninitialize();
        fatalMessageBox("Update WMI query failed");
    }

    IWbemClassObject* pclsObj{ nullptr };
    ULONG uReturn{ 0U };

    while (pEnumerator) {
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if(uReturn == 0) {
            break;
        }

        VARIANT vtProp;

        pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0);
        std::wcout << "Adapter Name : " << vtProp.bstrVal << std::endl;

        VariantClear(&vtProp);
    }
}

void NetMeasure::releaseWMI() {
    mp_services->Release();
    mp_locator->Release();
    CoUninitialize();
}*/

}