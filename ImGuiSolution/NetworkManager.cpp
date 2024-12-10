#include "NetworkManager.h"
#include "NetworkClient.h"
#include "NetworkPacket.h"
#include "Command.h"

NetworkManager::NetworkManager() : mClient(nullptr)
{
}

NetworkManager::~NetworkManager()
{
	DestroyNetwork();
}

bool NetworkManager::CreateNetwork()
{
	mClient = std::make_unique<NetworkClient>();
	mClient->mPacketCallback = [this](std::unique_ptr<NetworkPacket>&& packet) {
		OnReceivePacket(std::move(packet));
		};

	if (false == mClient->Initialize())
	{
		printf_s("Failed to initialize network client\n");
		return false;
	}

	// FIXME: Hardcoded IP and Port
	if (false == mClient->Connect("127.0.0.1", 9000)) {
		printf_s("Failed to connect to server\n");
		return false;
	}

	mClient->Run();

	return true;
}

void NetworkManager::DestroyNetwork()
{
	for (auto& client : mClientList)
	{
		client.reset();
	}

	mClientList.clear();

	WSACleanup();
}

bool NetworkManager::Send(const std::string& message) const
{
	return mClient->Send(message.c_str());
}

void NetworkManager::OnReceivePacket(std::unique_ptr<NetworkPacket>&& packet)
{
	switch ((ServiceProtocol)packet->Header.PacketID)
	{
		default:
		case ServiceProtocol::ECHO:
		{
			if (mCommandCallback)
			{
				mCommandCallback(Command(CommandType::Receive, (char*)packet->Body.data()));

				printf_s("Received: %s\n", packet->Body.data());
			}
			break;
		}
	}
}

