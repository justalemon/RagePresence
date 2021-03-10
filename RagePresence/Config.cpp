#include <fmt/format.h>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <spdlog/spdlog.h>
#include <unordered_set>

#include "Tools.h"

using namespace rapidjson;

std::unordered_set<std::string> makes = {};

bool IsMakeValid(std::string str)
{
	bool valid = makes.count(str);
	spdlog::get("file")->debug("Checking for {0}: {1}", str, valid);
	return valid;
}

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
		spdlog::get("file")->error("Configuration root object is not a JSON Object");
		ShowNotification("The root of the configuration file is not a JSON Object.");
		return;
	}

	// If there is a list of makes and is an array, update the existing list
	if (document.HasMember("makes"))
	{
		spdlog::get("file")->debug("Attempting to load list of Vehicle Makes...");
		Value& man = document["makes"];

		if (man.IsArray())
		{
			spdlog::get("file")->debug("Found valid aray, loading...");
			makes.clear();

			for (SizeType i = 0; i < man.Size(); i++)
			{
				Value& newMan = man[i];
				if (newMan.IsString())
				{
					std::string str = newMan.GetString();
					std::transform(str.begin(), str.end(), str.begin(), ::tolower);
					spdlog::get("file")->debug("Added manufacturer: {0}", str);
					makes.insert(str);
				}
			}
		}
	}
}
