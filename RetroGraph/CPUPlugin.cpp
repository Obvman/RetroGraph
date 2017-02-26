#include "CPUPlugin.h"

#include <iostream>

#include "utils.h"

namespace rg {

typedef bool (WINAPI *myGetCoreTempInfo)(CORE_TEMP_SHARED_DATA* pData);
myGetCoreTempInfo GetCoreTempInfo;

CPUPlugin::CPUPlugin() :
    m_ctData{ 0 },
    m_getCoreTempInfoSuccess{ false } {

    // Get the address of the GetCoreTempInfo function from the DLL
    const HMODULE hCT = LoadLibrary("GetCoreTempInfo.dll");
    if (!hCT) {
        fatalMessageBox("Could not load GetCoreTempInfo.dll");
    }

    GetCoreTempInfo = (myGetCoreTempInfo)GetProcAddress(hCT, "fnGetCoreTempInfoAlt");

    if (!GetCoreTempInfo) {
        fatalMessageBox("Could not get function address from GetCoreTempInfo.dll");
    }

    if (GetCoreTempInfo(&m_ctData)) {
        std::cout << "CPU Name: " << m_ctData.sCPUName << '\n';
        std::cout << "CPU Speed: " << m_ctData.fCPUSpeed << "MHz\n";
        std::cout << "CPU VID: " << m_ctData.fVID << "v\n";
        std::cout << "CPU count: " << m_ctData.uiCoreCnt << "\n";

        // Print current temps
        const auto tmpType = m_ctData.ucFahrenheit ? 'F' : 'C';
        for (auto i{ 0U }; i < m_ctData.uiCoreCnt; ++i) {
            std::cout << "Core #" << i << ": " << m_ctData.fTemp[i] << tmpType
                      << ", Load: " << m_ctData.uiLoad[i] << "%" << '\n';
        }
    } else {
        std::cout << "failed to get CoreTemp info\n";
    }
}


CPUPlugin::~CPUPlugin() {
}

void CPUPlugin::update() {
    m_getCoreTempInfoSuccess = GetCoreTempInfo(&m_ctData);

    /*if (GetCoreTempInfo(&m_ctData)) {
        const auto tmpType = m_ctData.ucFahrenheit ? 'F' : 'C';
        for (auto i{ 0U }; i < m_ctData.uiCoreCnt; ++i) {
            std::cout << "Core #" << i << ": " << m_ctData.fTemp[i] << tmpType
                      << ", Load: " << m_ctData.uiLoad[i] << "%" << '\n';
        }
    } else {
        std::cout << "failed to get CoreTemp info\n";
    }*/
}

}