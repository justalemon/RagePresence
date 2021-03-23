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

std::string lastZone = "";
std::string lastZoneName = "";

Ped lastPed = NULL;
Vehicle lastVehicle = NULL;

Hash lastMission = 0;
std::string lastMissionLabel = "";

bool changesDone = false;

void CheckForZone(std::string zone)
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

void CheckForMission()
{
	bool missionRunning = false;

	for (auto const& [script, label] : GetMissionList())
	{
		if (SCRIPT::GET_NUMBER_OF_REFERENCES_OF_SCRIPT_WITH_NAME_HASH_(script) > 0)
		{
			missionRunning = true;

			if (lastMission != script)
			{
				lastMission = script;
				lastMissionLabel = label;
				changesDone = true;
				//spdlog::get("file")->debug("Mission was changed to {0} (Label: {1})", script, label);
				return;
			}
		}
	}

	if (!missionRunning)
	{
		lastMission = 0;
		lastMissionLabel = "";
	}
}

void UpdatePresenceInfo(Ped ped, Vehicle vehicle, std::string zoneLabel)
{
	std::string zoneName = HUD::GET_LABEL_TEXT_(zoneLabel.c_str());
	std::string zoneLower = zoneLabel;
	std::transform(zoneLower.begin(), zoneLower.end(), zoneLower.begin(), ::tolower);

	std::string details = "";
	presence.state = "Freeroam";
	std::string smallImage = "";
	std::string smallText = "";

	if (lastMission != 0 && lastMissionLabel != "")
	{
		details = fmt::format("Playing {0}", HUD::GET_LABEL_TEXT_(lastMissionLabel.c_str()));
		presence.state = "On Mission";
	}
	else if (lastVehicle == NULL)
	{
		details = fmt::format("Walking down {0}", zoneName);
	}
	else
	{
		Hash entity = ENTITY::GET_ENTITY_MODEL(vehicle);
		std::string makeLabel = VEHICLE::GET_MAKE_NAME_FROM_VEHICLE_MODEL_(entity);
		std::transform(makeLabel.begin(), makeLabel.end(), makeLabel.begin(), ::tolower);
		std::string modelLabel = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(entity);

		details = fmt::format("Driving down {0}", zoneName);
		smallText = HUD::GET_LABEL_TEXT_(modelLabel.c_str());
		smallImage = fmt::format("man_{0}", GetMakeImage(makeLabel));
	}

	if (IsZoneValid(zoneLower))
	{
		std::string largeImage = fmt::format("zone_{0}", zoneLower);
		presence.largeImageKey = largeImage.c_str();
	}
	else
	{
		spdlog::get("file")->warn("Zone '{0}' is not valid", zoneLower);
		presence.largeImageKey = "zone_oceana";
	}

	presence.details = details.c_str();
	presence.smallImageKey = smallImage.c_str();
	presence.smallImageText = smallText.c_str();

	Discord_UpdatePresence(&presence);
}

void Init()
{
	// Initialize Discord
	spdlog::get("file")->debug("Initializing Discord Presence...");
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	memset(&presence, 0, sizeof(presence));
	Discord_Initialize(GetDiscordID(), &handlers, 1, "271590");

	// Now, set a random presence just to fill in some gaps
	presence.state = "Loading...";
	presence.details = "Waiting for the Game to load";
	presence.partySize = 1;
	presence.partyMax = 1;
	presence.startTimestamp = time(0);
	presence.largeImageKey = "zone_oceana";
	Discord_UpdatePresence(&presence);
}

void DoGameChecks()
{
	// Load the configuration (if possible) and initialize Discord
	Init();
	LoadConfig();

	// Wait until the playeable character can be controlled
	Player player = PLAYER::PLAYER_ID();
	while (DLC::GET_IS_LOADING_SCREEN_ACTIVE())
	{
		WAIT(0);
	}

	// Generate the cheat hash
	Hash cheatReload = MISC::GET_HASH_KEY("rpreload");
	Hash cheatReconnect = MISC::GET_HASH_KEY("rpreconnect");

	// Now, go ahead and start doing the checks
	while (true)
	{
		// If the user enters the "rpreload" cheat, reload the configuration
		if (MISC::HAS_CHEAT_STRING_JUST_BEEN_ENTERED_(cheatReload))
		{
			LoadConfig();
			changesDone = true;
		}
		// And for "rpreconnect", reconnect to Discord by reinitializing
		if (MISC::HAS_CHEAT_STRING_JUST_BEEN_ENTERED_(cheatReconnect))
		{
			Init();
			changesDone = true;
		}

		Ped ped = PLAYER::GET_PLAYER_PED(player);
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_IN(ped, false);
		Vector3 pos = ENTITY::GET_ENTITY_COORDS(ped, true);
		std::string zone = ZONE::GET_NAME_OF_ZONE(pos.x, pos.y, pos.z);

		CheckForZone(zone);
		CheckForPed(ped);
		CheckForVehicle(vehicle);
		CheckForMission();

		if (changesDone)
		{
			UpdatePresenceInfo(ped, vehicle, zone);
			spdlog::get("file")->debug("Presence updated at {0}", MISC::GET_GAME_TIMER());
			changesDone = false;
		}

		WAIT(0);
	}
}
