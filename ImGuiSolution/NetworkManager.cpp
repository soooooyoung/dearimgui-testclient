#include "NetworkManager.h"
#include "NetworkClient.h"
#include "NetworkPacket.h"

NetworkManager::NetworkManager() : mClient(nullptr)
{
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::CreateNetwork()
{
	mClient = std::make_unique<NetworkClient>();

	if (false == mClient->Initialize())
	{
		printf_s("Failed to initialize network client\n");
		return false;
	}

	if (false == mClient->Connect("127.0.0.1", 9000)) {
		printf_s("Failed to connect to server\n");
		return false;
	}

	mClient->Run();

	return true;
}

bool NetworkManager::Send(const std::string& message) const
{
	return mClient->Send(message.c_str());
}

