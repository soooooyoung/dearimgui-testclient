#pragma once
#include <string>
#include <memory>

enum class CommandType
{
	None,
	StartTest,
	StopTest,
	Connect,
	Send,
};

struct Command
{
	CommandType Type = CommandType::None;
	std::string Data;

	Command() {}
	Command(CommandType type, const char* data) : Type(type), Data(data ? data : "") {}
};