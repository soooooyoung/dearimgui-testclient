#include "NetworkClient.h"
#include "NetworkPacket.h"

NetworkClient::NetworkClient() :
	mSocket(INVALID_SOCKET),
	mIsConnected(false),
	mReceiveContext(nullptr),
	mSendContext(nullptr)
{
	mReceiveContext = std::make_unique<NetworkContext>();
	mSendContext = std::make_unique<NetworkContext>();
}

NetworkClient::~NetworkClient()
{
	WSACleanup();

	if (mSocket != INVALID_SOCKET)
	{
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}
}

bool NetworkClient::Initialize()
{
	SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == socket)
	{
		printf_s("WSASocket() Error on Network Client: %d\n", WSAGetLastError());
		return false;
	}

	mSocket = socket;
}

bool NetworkClient::PostConnect(const std::string& ipAddress, int port)
{
	DWORD bytes = 0;
	// ConnectEx Function Setup
	static LPFN_CONNECTEX ConnectEx = nullptr;
	if (ConnectEx == nullptr)
	{
		GUID guid = WSAID_CONNECTEX;
		
		// Get Extension Function Pointer
		if (WSAIoctl(mSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &ConnectEx, sizeof(ConnectEx), &bytes, nullptr, nullptr) != 0)
		{
			printf_s("WSAIoctl() Error on ConnectEx: %d\n", WSAGetLastError());
			return false;
		}
	}

	// Socket must be bound before using ConnectEx
	sockaddr_in localAddr = {};
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY; 
	localAddr.sin_port = 0; 

	if (SOCKET_ERROR == bind(mSocket, (SOCKADDR*)&localAddr, sizeof(localAddr)))
	{
		printf_s("bind() Error on Network Client: %d\n", WSAGetLastError());
		return false;
	}

	// Set Up Context
	mReceiveContext->ResetBuffer();
	mReceiveContext->ClearOverlapped();
	mReceiveContext->mContextType = ContextType::CONNECT;

	// Set Up Server Address
	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);

	if (FALSE == ConnectEx(mSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, 0, &bytes, (LPWSAOVERLAPPED)mReceiveContext.get()))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf_s("ConnectEx() Error on Network Client: %d\n", WSAGetLastError());
			return false;
		}
	}

	return true;
}

bool NetworkClient::OnConnect()
{
	mIsConnected = true;

	if (INVALID_SOCKET == mSocket)
	{
		printf_s("Invalid Socket\n");
		return false;
	}


	// Update socket's internal state
	// This is necessary to enable standard socket operations after using ConnectEx
	if (SOCKET_ERROR == setsockopt(mSocket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0))
	{
		printf_s("setsockopt() Error on Network Client: %d\n", WSAGetLastError());
		return false;
	}

	int flag = 1;

	// Disable Nagle's Algorithm
	if (SOCKET_ERROR == setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int)))
	{
		printf_s("setsockopt() Error on Network Client: %d\n", WSAGetLastError());
		return false;
	}

	if (false == Receive())
	{
		printf_s("Receive() Error\n");
		return false;
	}
}

bool NetworkClient::Send(const char* message)
{
	NetworkPacket packet;
	packet.Header.PacketID = 1;
	packet.Header.BodyLength = strlen(message);
	memcpy(packet.Body.data(), message, strlen(message));

	size_t totalSize = sizeof(NetworkPacket::PacketHeader) + packet.GetBodySize();

	mSendContext->ClearOverlapped();
	mSendContext->mContextType = ContextType::SEND;
	mSendContext->Write(packet);

	DWORD dwSendNumBytes = 0;
	WSABUF wsaBuf = { 0, };

	wsaBuf.len = static_cast<ULONG>(mSendContext->GetDataSize());
	wsaBuf.buf = reinterpret_cast<char*>(mSendContext->GetReadBuffer());

	int nRet = WSASend(mSocket,
		&wsaBuf,
		1,
		&dwSendNumBytes,
		0,
		(LPWSAOVERLAPPED)mSendContext.get(),
		NULL);

	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
	{
		printf("NetworkClient Send() fail: WSASend Error: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

std::unique_ptr<NetworkPacket> NetworkClient::GetPacket()
{

	auto remainSize = mReceiveContext->GetDataSize();

	if (remainSize < sizeof(NetworkPacket::PacketHeader))
	{
		printf("PacketHeader Size Error\n");
		return nullptr;
	}

	auto packetHeader = reinterpret_cast<NetworkPacket::PacketHeader*>(mReceiveContext->GetReadBuffer());
	auto packetLength = packetHeader->BodyLength + sizeof(NetworkPacket::PacketHeader);

	auto packet = std::make_unique<NetworkPacket>();
	packet->Header = *packetHeader;

	if (remainSize < packetLength)
	{
		printf("PacketBody Size Error\n");
		return nullptr;
	}

	memcpy(packet->Body.data(), mReceiveContext->GetReadBuffer() + sizeof(NetworkPacket::PacketHeader), packet->GetBodySize());


	if (false == mReceiveContext->Read(packet->GetPacketSize()))
	{
		printf("Read Error\n");
		return nullptr;
	}

	return packet;
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
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	mReceiveContext->mContextType = ContextType::RECV;
	mReceiveContext->ClearOverlapped();

	WSABUF wsaBuf = { 0, };
	wsaBuf.buf = reinterpret_cast<char*>(mReceiveContext->GetWriteBuffer());
	wsaBuf.len = mReceiveContext->GetRemainSize();

	int nRet = WSARecv(mSocket,
		&wsaBuf,
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED)mReceiveContext.get(),
		NULL);

	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError()))
	{
		printf("WSARecv Error : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

