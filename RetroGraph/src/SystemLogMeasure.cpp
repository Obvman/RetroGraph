#include "../headers/SystemLogMeasure.h"

#include <stdio.h>
#include <Windows.h>
#include <WinEvt.h>

#include "../headers/utils.h"

#pragma comment(lib, "Wevtapi.lib")

namespace rg {

void enumerateChannels();

SystemLogMeasure::SystemLogMeasure() {

}

SystemLogMeasure::~SystemLogMeasure() {

}

void SystemLogMeasure::init() {
    //enumerateChannels();

    /* DWORD bytesToRead = 0; */
    /* DWORD bytesRead = 0; */
    /* DWORD minBytesToRead = 0; */
    /* PBYTE tmpBuff = nullptr; */
    /* PBYTE buffer = (PBYTE)malloc(bytesToRead); */
    /* DWORD status = ERROR_SUCCESS; */

    //const auto hEventLog{ OpenEventLog(nullptr, "System") };

    /*if (!ReadEventLog(hEventLog, EVENTLOG_SEQUENTIAL_READ | EVENTLOG_FORWARDS_READ,
        0, buffer, bytesToRead, &bytesRead, &minBytesToRead)) {
        status = GetLastError();
        tmpBuff = (PBYTE)realloc(buffer, minBytesToRead);
        if (!tmpBuff) {
            fatalMessageBox("Realloc failed\n");
        }

        buffer = tmpBuff;
        bytesToRead = minBytesToRead;
    } else {
        if (status != ERROR_HANDLE_EOF) {
            fatalMessageBox("ReadEventLog failed");
        }
    }*/



}

void enumerateChannels() {
    // Enumerate event channels
    LPWSTR pBuffer{ nullptr };
    LPWSTR pTmp{ nullptr };
    DWORD bufferSize{ 0 };
    DWORD bufferUsed{ 0 };
    DWORD result{ ERROR_SUCCESS };

    const auto hChannels{ EvtOpenChannelEnum(nullptr, 0) };

    if (!hChannels) {
        fatalMessageBox("EvtOpenChannelEnum() failed\n");
    }

    printf("Channel list:\n");
    while (true) {
        if (!EvtNextChannelPath(hChannels, bufferSize, pBuffer, &bufferUsed)) {
            result = GetLastError();
            if (result == ERROR_NO_MORE_ITEMS) {
                break;
            } else if (result == ERROR_INSUFFICIENT_BUFFER) {
                bufferSize = bufferUsed;
                pTmp = (LPWSTR)realloc(pBuffer, bufferSize * sizeof(WCHAR));
                if (pTmp) {
                    pBuffer = pTmp;
                    pTmp = nullptr;
                    EvtNextChannelPath(hChannels, bufferSize, pBuffer, &bufferUsed);
                } else {
                    fatalMessageBox("realloc event buffer failed");
                }
            } else {
                fatalMessageBox("EvtNextChannelPath failed");
            }
        }

        wprintf(L"%s\n", pBuffer);
    }

    if (hChannels)
        EvtClose(hChannels);
    if (pBuffer)
        free(pBuffer);
}

void SystemLogMeasure::update(uint32_t) {
}

}
