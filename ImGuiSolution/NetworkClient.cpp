#include "NetworkClient.h"


NetworkClient::NetworkClient() : mSocket(INVALID_SOCKET)
{
}

NetworkClient::~NetworkClient()
{
	WSACleanup();
}

bool NetworkClient::Initialize()
{
	WSADATA wsaData;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		std::cerr << "Failed to initialize Winsock" << std::endl;
		return false;
	}

	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);

	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
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
		std::cerr << "Failed to connect to server: " << WSAGetLastError() << std::endl;
		return false;
	}

	if (setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, "1", sizeof(char)) == SOCKET_ERROR)
	{
		std::cerr << "Failed to set TCP_NODELAY: " << WSAGetLastError() << std::endl;
		return false;
	}

    return true;
}

void NetworkClient::Run()
{
	mIsRunning = true;
	mReceiveThread = std::thread(&NetworkClient::Receive, this);
}

bool NetworkClient::Send(const char* message)
{
    NetworkPacket packet;


	packet.Header.PacketID = 1;

	packet.Header.BodyLength = strlen(message);
	memcpy(packet.Body.data(), message, strlen(message));

	size_t totalSize = sizeof(NetworkPacket::PacketHeader) + packet.GetBodySize();
	int result = send(mSocket, reinterpret_cast<const char*>(&packet), totalSize, 0);

	return result != SOCKET_ERROR;
}

bool NetworkClient::Receive() const
{
	std::vector<uint8_t> buffer;
	buffer.resize(MAX_BUFFER_SIZE);

	while (true)
	{
		// Resize buffer to receive more data if necessary
		size_t currentSize = buffer.size();
		buffer.resize(currentSize + 1024); // Arbitrary chunk size for receiving more data

		// Read data from socket
		int received = recv(mSocket, reinterpret_cast<char*>(buffer.data() + currentSize), buffer.size() - currentSize, 0);
		if (received == SOCKET_ERROR)
		{
			std::cerr << "Failed to receive data: " << WSAGetLastError() << std::endl;
			return false;
		}

		if (received == 0)
		{
			std::cerr << "Connection closed" << std::endl;
			return false;
		}

		// Adjust buffer size to actual data received
		buffer.resize(currentSize + received);

		// Check if we have enough data for a header
		if (buffer.size() >= sizeof(NetworkPacket::PacketHeader))
		{
			// Interpret the header
			auto packetHeader = reinterpret_cast<NetworkPacket::PacketHeader*>(buffer.data());
			auto packetLength = packetHeader->BodyLength + sizeof(NetworkPacket::PacketHeader);

			printf_s("PacketLength: %d\n", packetLength);

			// Check if the full packet has been received
			if (buffer.size() >= packetLength)
			{
				auto packet = std::make_unique<NetworkPacket>();
				packet->Header = *packetHeader;
				std::memcpy(packet->Body.data(), buffer.data() + sizeof(NetworkPacket::PacketHeader), packetHeader->BodyLength);


				printf_s("PacketBody: %s\n", packet->GetBody());

				// TODO: Process packet

				buffer.erase(buffer.begin(), buffer.begin() + packetLength);

				return true;
			}
		}

	}
}

