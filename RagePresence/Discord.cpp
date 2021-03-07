#include <discord_rpc.h>
#include <memory.h>
#include <natives.hpp>
#include <stdio.h>
#include <string>
#include "Tools.h"

bool ready = false;
const DiscordUser* user = nullptr;

void __DiscordReady(const DiscordUser* connectedUser)
{
	ready = true;
	user = connectedUser;
}

void __DiscordErrored(int errorCode, const char* message)
{
	std::string s = std::to_string(errorCode);
	ShowNotification(s.c_str());
	ShowNotification(message);
}

void __DiscordDisconnected(int errorCode, const char* message)
{
	std::string s = std::to_string(errorCode);
	ShowNotification(s.c_str());
	ShowNotification(message);
}

void InitializeDiscord()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = __DiscordReady;
	handlers.errored = __DiscordErrored;
	handlers.disconnected = __DiscordDisconnected;
	Discord_Initialize("791461792382451752", &handlers, 1, "271590");
	ready = true;  // sadly, the ready event handler never gets called
}

bool IsReady()
{
	return ready;
}

void DoCleanup()
{
	Discord_Shutdown();
	ready = false;
}
