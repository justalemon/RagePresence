#include <natives.hpp>

void ShowNotification(const char* message)
{
	HUD::BEGIN_TEXT_COMMAND_THEFEED_POST("CELL_EMAIL_BCON");
	HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(message);
	HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(false, true);
}

void ShowDebugMessage(const char* message)
{
	#if DEBUG
	ShowNotification(message);
	#endif
}
