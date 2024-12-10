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

	mClientConfig.mServerPort = config.value("ServerPort", 0);
	mClientConfig.mServerAddress = config.value("ServerAddress", "127.0.0.1");

	return true;
}
