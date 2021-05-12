#pragma once

#define RP_API extern "C" __declspec(dllexport)

RP_API void SetCustomMission(const char* name);
RP_API bool IsCustomMissionSet();
RP_API void ClearCustomMission();

void DoGameChecks();
