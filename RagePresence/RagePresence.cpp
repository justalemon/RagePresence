#include <spdlog/spdlog.h>

#include "RagePresence.h"
#include "Globals.h"

void SetCustomMission(const char* mission)
{
	missionCustomName = mission;

	missionCustomSet = true;
	updateNextTick = true;

	spdlog::get("file")->debug("Mission Name was manually set to {0}", mission);
}

const char* GetCustomMission()
{
	return missionCustomName.c_str();
}

bool IsCustomMissionSet()
{
	return missionCustomSet;
}

void ClearCustomMission()
{
	missionCustomName = "";

	missionCustomSet = false;
	updateNextTick = true;

	spdlog::get("file")->debug("Mission Name was manually cleared");
}

void SetCustomDetails(const char* details)
{
	detailsCustomText = details;

	detailsCustomSet = false;
	updateNextTick = true;

	spdlog::get("file")->debug("RPC Details were manually set to {0}", details);
}

const char* GetCustomDetails()
{
	return detailsCustomText.c_str();
}

bool AreCustomDetailsSet()
{
	return detailsCustomSet;
}

void ClearCustomDetails()
{
	detailsCustomText = "";

	detailsCustomSet = false;
	updateNextTick = true;

	spdlog::get("file")->debug("Custom RPC Details were manually cleared");
}
