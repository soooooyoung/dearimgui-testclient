#include "NetworkManager.h"
#include "NetworkPacket.h"
#include "NetworkContext.h"
#include "Command.h"

NetworkManager::NetworkManager() : mIOCPHandle(NULL), mIsRunning(false)
{
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::CreateNetwork()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return false;
	}

	SYSTEM_INFO systemInfo;

	GetSystemInfo(&systemInfo);
	int ioThreadCount = systemInfo.dwNumberOfProcessors * 2;

	mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, ioThreadCount);

	if (NULL == mIOCPHandle)
	{
		printf_s("CreateIoCompletionPort() Error: %d\n", GetLastError());
		return false;
	}

	mIsRunning = true;

	for (int i = 0; i < ioThreadCount; ++i)
	{
		mIOThreadPool.emplace_back([this]() { WorkerThread(); });
	}


	return true;
}

std::shared_ptr<NetworkClient> NetworkManager::AddClient()
{
	auto client = std::make_shared<NetworkClient>();

	if (false == client->Initialize())
	{
		return nullptr;
	}

	auto hIOCP = CreateIoCompletionPort((HANDLE)client->GetSocket(), mIOCPHandle, (ULONG_PTR)client.get(), 0);

	if (NULL == hIOCP)
	{
		printf_s("CreateIoCompletionPort() Error: %d\n", GetLastError());
		return nullptr;
	}

	return client;
}

void NetworkManager::WorkerThread()
{
	BOOL bSuccess = TRUE;
	DWORD dwIoSize = 0;
	ULONG_PTR key = 0;
	LPOVERLAPPED lpOverlapped = NULL;

	while (mIsRunning)
	{
		bSuccess = GetQueuedCompletionStatus(mIOCPHandle, &dwIoSize, &key, &lpOverlapped, INFINITE);

		if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			mIsRunning = false;
			printf_s("WorkerThread Exit\n");
			continue;
		}

		if (nullptr == lpOverlapped)
		{
			printf_s("Invalid Overlapped\n");
			continue;
		}

		auto client = (NetworkClient*)key;
		auto context = (NetworkContext*)lpOverlapped;

		if (FALSE == bSuccess)
		{
			printf_s("WorkerThread Fail: %d\n", WSAGetLastError());
			continue;
		}

		switch (context->mContextType)
		{
		case ContextType::CONNECT:
		{
			_HandleConnect(*client, *context, dwIoSize);
		}
		break;
		case ContextType::RECV:
		{
			_HandleReceive(*client, *context, dwIoSize);
		}
		break;
		case ContextType::SEND:
		{
			_HandleSend(*client, *context, dwIoSize);
		}
		break;
		default:
		{
			printf("Unknown Context Type\n");
		}
		break;
		}
	}
}

void NetworkManager::_HandleConnect(NetworkClient& client, NetworkContext& context, int transferred)
{
	if (false == client.OnConnect())
	{
		printf_s("_HandleConnect Error: Failed to Connect\n");
		return;
	}
}

void NetworkManager::_HandleReceive(NetworkClient& client, NetworkContext& context, int transferred)
{
	if (transferred <= 0)
	{
		printf_s("_HandleSend Error: %d\n", WSAGetLastError());
		return;
	}

	if (false == client.ProcessPacket())
	{
		printf_s("_HandleSend Error: Failed to ProcessPacket\n");
		return;
	}
}

void NetworkManager::_HandleSend(NetworkClient& client, NetworkContext& context, int transferred)
{
	context.ResetBuffer();
}

