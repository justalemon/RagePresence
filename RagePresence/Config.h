#pragma once

#include <map>

const char* GetDiscordID();
std::string GetMakeImage(std::string str);
bool IsZoneValid(std::string);
std::map<Hash, std::string> GetMissionList();
void LoadConfig();
