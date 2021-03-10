#include <discord_rpc.h>
#include <fmt/format.h>
#include <main.h>
#include <natives.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <time.h>

#include "Config.h"
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
		spdlog::get("file")->debug("Zone was changed from '{0}' to '{1}'", lastZone, zone);
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
	spdlog::get("file")->debug("Ped was changed to {0} (Handle: {1})", ENTITY::GET_ENTITY_MODEL(ped), ped);
}

void CheckForVehicle(Vehicle vehicle)
{
	if (lastVehicle == vehicle)
	{
		return;
	}

	lastVehicle = vehicle;
	changesDone = true;
	spdlog::get("file")->debug("Vehicle changed to {0}", vehicle);
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
		std::string makeLabel = VEHICLE::GET_MAKE_NAME_FROM_VEHICLE_MODEL_(entity);
		std::transform(makeLabel.begin(), makeLabel.end(), makeLabel.begin(), ::tolower);
		const char* modelLabel = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(entity);

		details = fmt::format("Driving down {0}", zone);
		smallText = HUD::GET_LABEL_TEXT_(modelLabel);

		if (IsMakeValid(makeLabel))
		{
			smallImage = fmt::format("man_{0}", makeLabel);
		}
		else
		{
			spdlog::get("file")->debug("Make '{0}' is not valid", makeLabel);
			smallImage = "";  // TODO: Generic Vehicle class based images
		}
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
	// Initialize Discord
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	memset(&presence, 0, sizeof(presence));
	Discord_Initialize(GetDiscordID(), &handlers, 1, "271590");

	// Load the configuration (if possible)
	LoadConfig();

	// Now, set a random presence just to fill in some gaps
	presence.state = "Loading...";
	presence.details = "Waiting for the Game to load";
	presence.partySize = 1;
	presence.partyMax = 1;
	presence.startTimestamp = time(0);
	Discord_UpdatePresence(&presence);

	// Wait until the playeable character can be controlled
	Player player = PLAYER::PLAYER_ID();
	while (DLC::GET_IS_LOADING_SCREEN_ACTIVE())
	{
		WAIT(0);
	}

	// Generate the cheat hash
	Hash cheatHash = MISC::GET_HASH_KEY("rpreload");

	// Now, go ahead and start doing the checks
	while (true)
	{
		// If the user enters the "rpreload" cheat, load the configuration
		if (MISC::HAS_CHEAT_STRING_JUST_BEEN_ENTERED_(cheatHash))
		{
			LoadConfig();
			changesDone = true;
		}

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
			spdlog::get("file")->debug("Presence updated at {0}", MISC::GET_GAME_TIMER());
			changesDone = false;
		}

		WAIT(0);
	}
}
