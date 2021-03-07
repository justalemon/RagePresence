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
Hash lastPedModel = NULL;

Vehicle lastVehicle = NULL;
Hash lastVehicleModel = NULL;
const char* lastVehicleMake = "";

bool changesDone = false;

void CheckForZone(const char* zone)
{
	if (lastZone != zone)
	{
		ShowDebugMessage(fmt::format("Zone was changed from {0} to {1}", lastZone, zone).c_str());
		lastZone = zone;
		lastZoneName = HUD::GET_LABEL_TEXT_(zone);
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
	lastPedModel = ENTITY::GET_ENTITY_MODEL(ped);
	changesDone = true;
	ShowDebugMessage(fmt::format("Ped was changed to {0} (Handle: {1})", lastPedModel, ped).c_str());
}

void CheckForVehicle(Vehicle vehicle)
{
	if (lastVehicle == vehicle)
	{
		return;
	}

	lastVehicle = vehicle;
	lastVehicleModel = ENTITY::GET_ENTITY_MODEL(vehicle);
	lastVehicleMake = HUD::GET_LABEL_TEXT_(VEHICLE::GET_MAKE_NAME_FROM_VEHICLE_MODEL_(lastVehicleModel));
	changesDone = true;
	ShowDebugMessage(fmt::format("Vehicle was changed to {0} (Handle: {1})", lastPedModel, vehicle).c_str());
}

void UpdatePresenceInfo()
{
	std::string details = "";
	if (lastVehicle == NULL)
	{
		details = fmt::format("Walking down {0}", lastZoneName);
	}
	else
	{
		details = fmt::format("Driving down {0}", lastZoneName);
	}
	presence.details = details.c_str();
	presence.state = "Free Mode";
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
	presence.details = "Waiting for the Game to load";
	presence.state = "Loading...";
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
			UpdatePresenceInfo();
			ShowDebugMessage(fmt::format("Presence updated at {0}", MISC::GET_GAME_TIMER()).c_str());
			changesDone = false;
		}

		WAIT(0);
	}
}
