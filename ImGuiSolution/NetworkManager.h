#pragma once
#include <thread>
#include <vector>

#include "NetworkClient.h"
#include "SharedEnum.h"

struct Command;
struct NetworkPacket;
class NetworkContext;
class PacketHelper;
class NetworkManager
{
public:
	NetworkManager();
	virtual ~NetworkManager();
	bool CreateNetwork();

	std::shared_ptr<NetworkClient> AddClient();
private:
	void WorkerThread();

	void _HandleConnect(NetworkClient& client, NetworkContext& context, int transferred);
	void _HandleReceive(NetworkClient& client, NetworkContext& context, int transferred);
	void _HandleSend(NetworkClient& client, NetworkContext& context, int transferred);

	bool mIsRunning = false;
	std::vector<std::thread> mIOThreadPool;
	HANDLE mIOCPHandle;
};