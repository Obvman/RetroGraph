// dllmain.cpp : Defines the entry point for the DLL application.

import "WindowsHeaders.h";

static HMODULE dllHandle;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID /*lpReserved*/)
{
    dllHandle = hModule;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HMODULE GetResourceHandle() {
    return dllHandle;
}
