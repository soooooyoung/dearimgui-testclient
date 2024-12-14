#pragma once
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <Windows.h>

#include <string>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <functional>


struct NetworkPacket;
class NetworkContext;
class NetworkClient {
	 
public:
	NetworkClient();
	virtual ~NetworkClient();

	SOCKET GetSocket() const { return mSocket; }

	bool Initialize();

	bool PostConnect(const std::string& ipAddress, int port);
	bool OnConnect();

	bool Receive();
	bool Send(const char* message);

	bool ProcessPacket();

	std::unique_ptr<NetworkPacket> GetPacket();

	std::function<void(std::unique_ptr<NetworkPacket>&&)> mPacketCallback;
private:
	bool mIsConnected = false;
	SOCKET mSocket;

	uint32_t mSessionID = 0;

	std::unique_ptr<NetworkContext> mReceiveContext;
	std::unique_ptr<NetworkContext> mSendContext;
};