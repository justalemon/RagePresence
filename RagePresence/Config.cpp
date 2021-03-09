#include <fmt/format.h>
#include <fstream>
#include <rapidjson/document.h>
#include "rapidjson/error/en.h"
#include <rapidjson/istreamwrapper.h>
#include <set>

#include "Tools.h"

using namespace rapidjson;

void LoadConfig()
{
	// Try to open the file
	std::ifstream ifs("RagePresence.json");
	// If it couldn't be opened, return
	if (!ifs.is_open())
	{
		ShowNotification("The configuration could not be loaded. Make sure that the file exists.");
		return;
	}

	// Wrap the stream
	IStreamWrapper isw(ifs);

	// Try to parse the document
	Document document{};
	document.ParseStream<kParseCommentsFlag>(isw);
	// If there was a parsing error, return
	if (document.HasParseError())
	{
		ShowNotification("Unable to parse the configuration file. Please make sure that the format is valid.");
		std::string error = fmt::format("{0}: {1}", document.GetErrorOffset(), GetParseError_En(document.GetParseError()));
		ShowNotification(error.c_str());
		return;
	}

	// If the document is not an object
	if (!document.IsObject())
	{
		ShowNotification("The root of the configuration file is not a JSON object.");
		return;
	}
}
