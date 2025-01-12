#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

HMODULE g_ModuleHandle = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        g_ModuleHandle = hModule;
        break;
    case DLL_PROCESS_DETACH:
        g_ModuleHandle = nullptr;
        break;
    default:
        break;
    }

    return TRUE;
}