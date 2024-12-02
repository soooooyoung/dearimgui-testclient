#pragma once

#include <WinSock2.h>
#include <string>
#include <iostream>
#include <WS2tcpip.h>
#include <thread>
#include <MSWSock.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

const int MAX_BUFFER_SIZE = 8192;
#include "NetworkPacket.h"

class NetworkClient {
	 
public:
	NetworkClient();
	virtual ~NetworkClient();

	bool Initialize();
	bool Connect(const std::string& ipAddress, int port);
	void Run();

	bool Send(const char* message);
	bool Receive() const;
private:


	SOCKET mSocket;
	std::thread mReceiveThread;
	bool mIsRunning = false;
};