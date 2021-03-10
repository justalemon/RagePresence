#include <main.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <windows.h>

#include "GameChecks.h"

void Attach(HMODULE hModule)
{
    auto logger = spdlog::basic_logger_mt("file", "RagePresence.log", true);
    spdlog::flush_every(std::chrono::seconds(1));
#if DEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif
    logger->info("Starting RagePresence...");
    scriptRegister(hModule, DoGameChecks);
}

void Detach(HMODULE hModule)
{
    scriptUnregister(hModule);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            Attach(hModule);
            break;
        case DLL_PROCESS_DETACH:
            Detach(hModule);
            break;
    }
    return TRUE;
}
