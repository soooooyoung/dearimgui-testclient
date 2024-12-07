#include "NetworkClient.h"
#include "NetworkPacket.h"
#include "NetworkContext.h"

NetworkClient::NetworkClient() : 
	mSocket(INVALID_SOCKET), 
	mIsRunning(false), 
	mReceiveContext(nullptr)
{
	mReceiveContext = std::make_unique<NetworkContext>();
}

NetworkClient::~NetworkClient()
{
	WSACleanup();

	if (mSocket != INVALID_SOCKET)
	{
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}

	if (mReceiveThread.joinable())
	{
		mReceiveThread.join();
	}
}

bool NetworkClient::Initialize()
{
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf_s("Failed to initialize Winsock: %d\n", WSAGetLastError());
		return false;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf_s("Failed to create socket: %d\n", WSAGetLastError());
		return false;
	}

	mSocket = sock;

	return true;
}

bool NetworkClient::Connect(const std::string& ipAddress, int port) 
{
	if (mSocket == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr);
	serverAddress.sin_port = htons(port);

	if (SOCKET_ERROR == connect(mSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)))
	{
		printf_s("Failed to connect to server: %d\n", WSAGetLastError());
		return false;
	}

	// Disable Nagle's algorithm
	int flag = 1;
	if (setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(char)) == SOCKET_ERROR)
	{
		printf_s("Failed to set TCP_NODELAY: %d\n", WSAGetLastError());
		return false;
	}

    return true;
}

void NetworkClient::Run()
{
	mIsRunning = true;
	mReceiveThread = std::thread(&NetworkClient::ReceiveThread, this);
}

bool NetworkClient::Send(const char* message)
{
    NetworkPacket packet;
	packet.Header.PacketID = 1;
	packet.Header.BodyLength = strlen(message);
	memcpy(packet.Body.data(), message, strlen(message));

	size_t totalSize = sizeof(NetworkPacket::PacketHeader) + packet.GetBodySize();
	int result = send(mSocket, reinterpret_cast<const char*>(&packet), totalSize, 0);

	if (result == SOCKET_ERROR)
	{
		printf_s("Failed to send data: %d\n", WSAGetLastError());
		return false;
	}

	if (result != totalSize)
	{
		printf_s("Failed to send all data: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool NetworkClient::ReceiveThread()
{
	while (mIsRunning)
	{
		if (false == Receive())
		{
			Sleep(100);
		}
	}

	return true;
}

bool NetworkClient::ProcessPacket()
{
	auto remainSize = mReceiveContext->GetDataSize();

	if (remainSize < sizeof(NetworkPacket::PacketHeader))
	{
		printf_s("PacketHeader Size Error\n");
		return false;
	}

	auto packetHeader = reinterpret_cast<NetworkPacket::PacketHeader*>(mReceiveContext->GetReadBuffer());
	auto packetLength = packetHeader->BodyLength + sizeof(NetworkPacket::PacketHeader);

	auto packet = std::make_unique<NetworkPacket>();
	packet->Header = *packetHeader;

	if (remainSize < packetLength)
	{
		printf_s("PacketBody Size Error\n");
		return false;
	}

	memcpy(packet->Body.data(), mReceiveContext->GetReadBuffer() + sizeof(NetworkPacket::PacketHeader), packet->GetBodySize());


	if (false == mReceiveContext->Read(packet->GetPacketSize()))
	{
		printf_s("Read Error\n");
		return false;
	}

	mPacketCallback(std::move(packet));
}

bool NetworkClient::Receive()
{
	auto result = recv(mSocket, reinterpret_cast<char*>(mReceiveContext->GetWriteBuffer()), mReceiveContext->GetRemainSize(), 0);

	if (result == SOCKET_ERROR)
	{
		printf_s("Failed to receive data: %d\n", WSAGetLastError());
		return false;
	}

	if (result == 0)
	{
		printf_s("Connection closed\n");
		return false;
	}

	mReceiveContext->Write(result);

	if (false == ProcessPacket())
	{
		printf_s("Failed to process packet\n");
		return false;
	}
}

