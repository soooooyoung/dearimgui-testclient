#pragma once

enum class ContextType
{
	NONE,
	CONNECT,
	RECV,
	SEND,
};

enum class ServiceProtocol
{
	NONE,
	ECHO,
};