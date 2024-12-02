#pragma once
#include <memory>

class NetworkClient;
class NetworkManager
{
public:
	NetworkManager();
	virtual ~NetworkManager();
	bool CreateNetwork();
private:
	std::unique_ptr<NetworkClient> mClient;
};