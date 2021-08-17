#include <main.h>
#include <natives.hpp>
#include <string>
#include <map>
#include "Config.h"

int lastCheck = 0;
std::string agencyName = "";

bool UpdateAgencyName(bool force)
{
	if (MISC::GET_GAME_TIMER() < lastCheck + (15 * 60 * 1000) && !force)
	{
		return false;
	}

	int peds[1024];
	std::map<std::string, int> found;

	int actualNumberOfPeds = worldGetAllPeds(peds, 1024);

	for (int i = 0; i < actualNumberOfPeds; ++i) {
		Ped ped = peds[i];
		std::string agency = GetAgencyOfPed(ped);

		if (agency == "")
		{
			continue;
		}

		std::map<std::string, int>::iterator itr = found.find(agency);

		if (itr == found.end()) // Not Present
		{
			std::pair<std::string, int> pair(agency, 1);
			found.insert(pair);
		}
		else
		{
			itr->second = itr->second + 1;
		}
	}

	int biggest = 0;
	std::string text = "";

	for (auto const& [name, count] : found)
	{
		if (count > biggest)
		{
			biggest = count;
			text = name;
		}
	}

	agencyName = text;
	lastCheck = MISC::GET_GAME_TIMER();

	return true;
}

bool IsChaseActive()
{
	return PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()) > 0;
}

std::string GetAgencyName()
{
	return agencyName;
}
