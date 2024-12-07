#pragma once
#include <memory>
#include <string>
#include <thread>
#include <queue>
#include <functional>

#include "SharedEnum.h"

struct Command;
struct NetworkPacket;
class NetworkClient;
class NetworkManager
{
public:
	NetworkManager();
	virtual ~NetworkManager();
	bool CreateNetwork();

	bool Send(const std::string& message) const;

	void OnReceivePacket(std::unique_ptr<NetworkPacket>&& packet);

	std::function<void(Command&&)> mCommandCallback;
private:
	std::unique_ptr<NetworkClient> mClient;
};