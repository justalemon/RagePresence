#include <fmt/format.h>
#include <fstream>
#include <map>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <spdlog/spdlog.h>
#include <types.h>
#include <unordered_set>

#include "Tools.h"
#include <natives.hpp>

using namespace rapidjson;

const char* default_client_id = "791461792382451752";
std::string default_make_image = "";

const char* client_id = default_client_id;
std::map<std::string, std::string> makes = {};
std::unordered_set<std::string> zones = {};
std::map<Hash, std::string> missions = {};
std::map<Hash, std::string> agencies = {};

const char* GetDiscordID()
{
	return client_id;
}

std::string GetMakeImage(std::string str)
{
	if (makes.count(str) > 0)
	{
		std::string found = makes[str];
		spdlog::get("file")->debug("Returning image {0} for make {1}", found, str);
		return found;
	}
	spdlog::get("file")->debug("Image not found for {0}, returning default ({1})", str, default_make_image);
	return default_make_image;
}

bool IsZoneValid(std::string str)
{
	bool valid = zones.count(str);
	spdlog::get("file")->debug("Checking for Zone {0}: {1}", str, valid);
	return valid;
}

std::map<Hash, std::string> GetMissionList()
{
	return missions;
}

std::string GetAgencyOfPed(Ped ped)
{
	if (ENTITY::GET_ENTITY_TYPE(ped) != 1)
	{
		return "";
	}

	Hash model = ENTITY::GET_ENTITY_MODEL(ped);

	if (agencies.count(model))
	{
		return agencies[model];
	}
	else
	{
		switch (PED::GET_PED_TYPE(ped))
		{
			case 27:
				return "the SWAT";
			case 29:
				return "the Army";
			case 6:
				return "the Police";
			default:
				return "";
		}
	}
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

	// If there is a Client ID, use it
	if (document.HasMember("client_id"))
	{
		Value& id = document["client_id"];

		if (id.IsString())
		{
			const char* newId = id.GetString();
			spdlog::get("file")->info("Using Client ID from configuration ({0})", newId);
			client_id = newId;
		}
		else
		{
			spdlog::get("file")->warn("Client ID in configuration is not a valid String!");
			spdlog::get("file")->info("Using default Client ID from Lemon ({0})", default_client_id);
			client_id = default_client_id;
		}
	}
	else
	{
		spdlog::get("file")->info("Using default Client ID from Lemon ({0})", default_client_id);
		client_id = default_client_id;
	}

	// Add the zones if present
	if (document.HasMember("zones"))
	{
		spdlog::get("file")->debug("Attempting to load list of Zones...");
		Value& zon = document["zones"];

		if (zon.IsArray())
		{
			spdlog::get("file")->debug("Found valid aray, loading...");
			zones.clear();

			for (SizeType i = 0; i < zon.Size(); i++)
			{
				Value& newZone = zon[i];
				if (newZone.IsString())
				{
					std::string str = newZone.GetString();
					std::transform(str.begin(), str.end(), str.begin(), ::tolower);
					spdlog::get("file")->debug("Added zone: {0}", str);
					zones.insert(str);
				}
			}
		}
	}

	// If there is a list of makes and is an array, update the existing list
	if (document.HasMember("makes"))
	{
		spdlog::get("file")->debug("Attempting to load list of Vehicle Makes...");
		Value& man = document["makes"];

		if (man.IsObject())
		{
			spdlog::get("file")->debug("Found valid object, loading...");
			makes.clear();

			for (Value::ConstMemberIterator child = man.MemberBegin(); child != man.MemberEnd(); ++child)
			{
				if (child->value.IsString())
				{
					std::string label = child->name.GetString();
					std::transform(label.begin(), label.end(), label.begin(), ::tolower);
					std::pair<std::string, std::string> pair(label, child->value.GetString());
					makes.insert(pair);
					spdlog::get("file")->debug("Found Make '{0}' with Image '{1}'", pair.first, pair.second);
				}
			}
		}
	}

	// If there is a valid list of missions, load it
	if (document.HasMember("missions"))
	{
		spdlog::get("file")->debug("Attempting to load list of Missions...");
		Value& miss = document["missions"];

		if (miss.IsObject())
		{
			missions.clear();

			for (Value::ConstMemberIterator child = miss.MemberBegin(); child != miss.MemberEnd(); ++child)
			{
				if (child->value.IsString())
				{
					const char* mission = child->name.GetString();
					std::pair<Hash, std::string> pair(MISC::GET_HASH_KEY(mission), child->value.GetString());
					missions.insert(pair);
					spdlog::get("file")->debug("Found Mission Script '{0}' ({1}) with Text Label '{2}'", mission, pair.first, pair.second);
				}
			}
		}
		else
		{
			spdlog::get("file")->warn("List of missions is not an Array!");
		}
	}

	if (document.HasMember("agencies"))
	{
		spdlog::get("file")->debug("Attempting to load corresponding Ped Agencies...");
		Value& rawAgencies = document["agencies"];

		if (rawAgencies.IsObject())
		{
			agencies.clear();

			for (Value::ConstMemberIterator child = rawAgencies.MemberBegin(); child != rawAgencies.MemberEnd(); ++child)
			{
				if (child->value.IsString())
				{
					const char* model = child->name.GetString();
					std::pair<Hash, std::string> pair(MISC::GET_HASH_KEY(model), child->value.GetString());
					agencies.insert(pair);
					spdlog::get("file")->debug("Found Agency for Ped '{0}' ({1}) called '{2}'", model, pair.first, pair.second);
				}
			}
		}
		else
		{
			spdlog::get("file")->warn("List of Ped Agencies is not an Array!");
		}
	}
}
