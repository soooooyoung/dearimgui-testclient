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
#include <concurrent_queue.h>

#include "NetworkContext.h"

struct NetworkPacket;
class NetworkClient {
public:
	NetworkClient();
	virtual ~NetworkClient();

	bool Initialize();
	void Close();

	bool PostConnect(const std::string& ipAddress, int port);
	bool OnConnect();
	bool Receive();
	bool Send();

	bool ProcessPacket();

	SOCKET GetSocket() const { return mSocket; }

	int GetSessionID() const { return mSessionID; }
	void SetSessionID(int sessionID) { mSessionID = sessionID; }
	
	bool IsConnected() const { return mIsConnected; }
	bool IsPacketAvailable() { return !mPacketQueue.empty(); }

	std::shared_ptr<NetworkPacket> GetPacket() {
		std::shared_ptr<NetworkPacket> packet;
		mPacketQueue.try_pop(packet);
		return packet;
	}

	void PushChat(const std::string& chat) { mChatHistory.push_back(chat); }	
	std::vector<std::string>& GetChatHistory() { return mChatHistory; }

	std::unique_ptr<NetworkContext>& GetSendContext() { return mSendContext; }	
private:
	bool mIsConnected = false;
	int mSessionID = 0;

	SOCKET mSocket;

	std::unique_ptr<NetworkContext> mReceiveContext;
	std::unique_ptr<NetworkContext> mSendContext;

	concurrency::concurrent_queue<std::shared_ptr<NetworkPacket>> mPacketQueue;

	std::vector<std::string> mChatHistory;
};