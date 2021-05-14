#include <spdlog/spdlog.h>

#include "RagePresence.h"
#include "Globals.h"

void SetCustomMission(const char* name)
{
	missionCustomName = name;

	missionCustomSet = true;
	updateNextTick = true;

	spdlog::get("file")->debug("Mission Name was manually set to {0}", name);
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
