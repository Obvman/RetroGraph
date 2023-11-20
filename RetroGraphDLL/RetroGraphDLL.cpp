// RetroGraphDLL.cpp : Defines the entry point for the DLL application.

import "WindowsHeaderUnit.h";

static HMODULE dllHandle;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD /*ul_reason_for_call*/, LPVOID /*lpReserved*/)
{
    dllHandle = hModule;
    return TRUE;
}

HMODULE GetResourceHandle() {
    return dllHandle;
}
