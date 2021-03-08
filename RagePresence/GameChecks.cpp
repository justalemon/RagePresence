#include <discord_rpc.h>
#include <fmt/format.h>
#include <main.h>
#include <natives.hpp>
#include <string>
#include <time.h>

#include "Discord.h"
#include "Tools.h"

DiscordRichPresence presence;

const char* lastZone = "";
const char* lastZoneName = "";

Ped lastPed = NULL;
Vehicle lastVehicle = NULL;

bool changesDone = false;

void CheckForZone(const char* zone)
{
	if (lastZone != zone)
	{
		ShowDebugMessage(fmt::format("Zone was changed from {0} to {1}", lastZone, zone).c_str());
		lastZone = zone;
		changesDone = true;
	}
}

void CheckForPed(Ped ped)
{
	if (lastPed == ped)
	{
		return;
	}

	lastPed = ped;
	changesDone = true;
	ShowDebugMessage(fmt::format("Ped was changed to {0} (Handle: {1})", ENTITY::GET_ENTITY_MODEL(ped), ped).c_str());
}

void CheckForVehicle(Vehicle vehicle)
{
	if (lastVehicle == vehicle)
	{
		return;
	}

	lastVehicle = vehicle;
	changesDone = true;
	ShowDebugMessage(fmt::format("Vehicle changed to:\nHandle: {0}", vehicle).c_str());
}

void UpdatePresenceInfo(Ped ped, Vehicle vehicle, const char* zoneLabel)
{
	const char* zone = HUD::GET_LABEL_TEXT_(zoneLabel);

	std::string details = "";
	std::string smallImage = "";
	std::string smallText = "";

	if (lastVehicle == NULL)
	{
		details = fmt::format("Walking down {0}", zone);
	}
	else
	{
		Hash entity = ENTITY::GET_ENTITY_MODEL(vehicle);
		const char* makeLabel = VEHICLE::GET_MAKE_NAME_FROM_VEHICLE_MODEL_(entity);
		const char* modelLabel = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(entity);

		std::string make = makeLabel;
		std::transform(make.begin(), make.end(), make.begin(), ::tolower);

		details = fmt::format("Driving down {0}", zone);
		smallText = HUD::GET_LABEL_TEXT_(modelLabel);
		smallImage = fmt::format("man_{0}", make);
	}

	std::string zoneLower = zoneLabel;
	std::transform(zoneLower.begin(), zoneLower.end(), zoneLower.begin(), ::tolower);
	std::string largeImage = fmt::format("zone_{0}", zoneLower);

	presence.state = "Free Mode";
	presence.details = details.c_str();
	presence.largeImageKey = largeImage.c_str();
	presence.smallImageKey = smallImage.c_str();
	presence.smallImageText = smallText.c_str();

	Discord_UpdatePresence(&presence);
}

void DoGameChecks()
{
	// Empty the presence information
	memset(&presence, 0, sizeof(presence));

	// Wait until Discord RPC is initialized
	while (!IsReady())
	{
		WAIT(0);
	}

	// Now, set a random presence just to fill in some gaps
	presence.state = "Loading...";
	presence.details = "Waiting for the Game to load";
	presence.partySize = 1;
	presence.partyMax = 1;
	presence.startTimestamp = time(0);
	Discord_UpdatePresence(&presence);

	// Wait until the playeable character can be controlled
	Player player = PLAYER::PLAYER_ID();
	while (DLC::GET_IS_LOADING_SCREEN_ACTIVE() || !PLAYER::IS_PLAYER_CONTROL_ON(player))
	{
		WAIT(0);
	}

	// Now, go ahead and start doing the checks
	while (true)
	{
		Ped ped = PLAYER::GET_PLAYER_PED(player);
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_IN(ped, false);
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, true);
		const char* zone = ZONE::GET_NAME_OF_ZONE(pos.x, pos.y, pos.z);

		CheckForZone(zone);
		CheckForPed(ped);
		CheckForVehicle(vehicle);

		if (changesDone)
		{
			UpdatePresenceInfo(ped, vehicle, zone);
			ShowDebugMessage(fmt::format("Presence updated at {0}", MISC::GET_GAME_TIMER()).c_str());
			changesDone = false;
		}

		WAIT(0);
	}
}
