#include <main.h>
#include <windows.h>
#include "Discord.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            scriptRegister(hModule, InitializeDiscord);
            break;
        case DLL_PROCESS_DETACH:
            scriptUnregister(hModule);
            DoCleanup();
            break;
    }
    return TRUE;
}
