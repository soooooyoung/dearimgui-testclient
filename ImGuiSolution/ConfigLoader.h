#pragma once
#include <string>
#include <array>
#include <unordered_map>

/* Client Configuration */
struct ClientConfig
{
	int mServerPort = 0;
	std::string mServerAddress = "";
};

/* Test Configuration */
struct TestConfig
{
	int mClientCount = 0;
	int mInterval = 0;
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

	const TestConfig& GetTestConfig() const
	{
		return mTestConfig;
	}
private:
	std::string mConfigPath = "config.json";

	ClientConfig mClientConfig;
	TestConfig mTestConfig;
};