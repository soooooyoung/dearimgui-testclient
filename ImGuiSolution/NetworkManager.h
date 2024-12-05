#pragma once
#include <memory>
#include <string>
#include <thread>

class NetworkClient;
class NetworkManager
{
public:
	NetworkManager();
	virtual ~NetworkManager();
	bool CreateNetwork();

	bool Send(const std::string& message) const;
private:
	std::unique_ptr<NetworkClient> mClient;
};