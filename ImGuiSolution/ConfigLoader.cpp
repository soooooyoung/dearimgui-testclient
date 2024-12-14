#include "ConfigLoader.h"
#include "JSON.h"

ConfigLoader::ConfigLoader()
{
}

ConfigLoader::~ConfigLoader()
{
}

bool ConfigLoader::Load()
{
	auto config = JSON::ReadJsonFromFile(mConfigPath);

	if (config.is_null())
	{
		return false;
	}

	auto& server = config["server"];

	if (server.is_null())
	{
		return false;
	}

	mClientConfig.mServerAddress = server.value<std::string>("ip", "");
	mClientConfig.mServerPort = server.value<int>("port", 0);

	return true;
}
