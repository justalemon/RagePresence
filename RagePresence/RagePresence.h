#pragma once

#define RP_API extern "C" __declspec(dllexport)

RP_API void SetCustomMission(const char* mission);
RP_API const char* GetCustomMission();
RP_API bool IsCustomMissionSet();
RP_API void ClearCustomMission();

RP_API void SetCustomDetails(const char* details);
RP_API const char* GetCustomDetails();
RP_API bool AreCustomDetailsSet();
RP_API void ClearCustomDetails();

RP_API void SetCustomState(const char* state);
RP_API const char* GetCustomState();
RP_API bool IsCustomStateSet();
RP_API void ClearCustomState();
