#pragma once

#include <map>

const char* GetDiscordID();
bool IsMakeValid(std::string);
bool IsZoneValid(std::string);
std::map<Hash, std::string> GetMissionList();
void LoadConfig();
