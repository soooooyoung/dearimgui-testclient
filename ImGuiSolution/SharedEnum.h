#pragma once

enum class ContextType
{
	NONE,
	ACCEPT,
	RECV,
	SENDQUEUE,
	SEND,
};

enum class ServiceProtocol
{
	NONE,
	ECHO,
};