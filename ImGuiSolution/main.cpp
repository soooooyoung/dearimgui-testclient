#pragma once
#include <WinSock2.h>
#include <iostream>


#include "DirectWindow.h"
#include "NetworkClient.h"
#include "NetworkPacket.h"

int main(int, char**)
{
	NetworkClient networkClient;
	if (false == networkClient.Initialize())
	{
		std::cerr << "Failed to initialize network manager" << std::endl;
		return 1;
	}

	if (false == networkClient.Connect("127.0.0.1", 9000)) {
		std::cerr << "Failed to connect to server at 127.0.0.1:9000" << std::endl;
		return -1;
	}

	networkClient.Run();
	const char* testmessage = "Hello, Server!";

	if (false == networkClient.Send(testmessage))
	{
		std::cerr << "Failed to send message" << std::endl;
		return -1;
	}
	std::cout << "Connected to server at 127.0.0.1:9000" << std::endl;

	DirectWindow windowManager;

	if (false == windowManager.Initialize())
	{
		return 1;
	}

	windowManager.RunLoop();

	return 0;
}
