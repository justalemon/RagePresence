#include <main.h>
#include <windows.h>
#include "Discord.h"
#include "GameChecks.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            scriptRegister(hModule, InitializeDiscord);
            scriptRegister(hModule, DoGameChecks);
            break;
        case DLL_PROCESS_DETACH:
            scriptUnregister(hModule);
            DoCleanup();
            break;
    }
    return TRUE;
}
