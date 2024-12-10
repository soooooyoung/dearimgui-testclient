#pragma once
#include <string>
#include <array>
#include <unordered_map>


struct ClientConfig
{
	int mServerPort = 0;
	std::string mServerAddress = "";
};


/* Configuration Loader */
class ConfigLoader
{
public:
	ConfigLoader();
	~ConfigLoader();

	bool Load();

	static ConfigLoader& GetInstance()
	{
		static ConfigLoader instance;
		return instance;
	}

	const ClientConfig& GetClientConfig() const
	{
		return mClientConfig;
	}

private:
	std::string mConfigPath = "config.json";

	ClientConfig mClientConfig;
};