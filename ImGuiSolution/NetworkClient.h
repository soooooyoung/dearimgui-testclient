#pragma once

#include <WinSock2.h>
#include <string>
#include <iostream>
#include <WS2tcpip.h>
#include <thread>
#include <MSWSock.h>
#include <memory>
#include <vector>
#include <functional>

#pragma comment(lib, "ws2_32.lib")

const int MAX_BUFFER_SIZE = 8192;

struct NetworkPacket;
class NetworkContext;
class NetworkClient {
	 
public:
	NetworkClient();
	virtual ~NetworkClient();

	bool Initialize();
	bool Connect(const std::string& ipAddress, int port);
	void Run();

	bool Receive();
	bool Send(const char* message);

	bool ProcessPacket();
	bool ReceiveThread();

	std::function<void(std::unique_ptr<NetworkPacket>&&)> mPacketCallback;
private:
	SOCKET mSocket;

	std::unique_ptr<NetworkContext> mReceiveContext;
	std::thread mReceiveThread;

	bool mIsRunning = false;
};