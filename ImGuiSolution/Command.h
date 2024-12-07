#pragma once
#include <string>
#include <memory>

enum class CommandType
{
	None,
	Connect,
	Send,
	Receive,
};

struct Command
{
	CommandType Type = CommandType::None;
	std::string Data;

	Command() {}
	Command(CommandType type, const char* data) : Type(type), Data(data ? data : "") {}

	Command(const Command&) = delete;
	Command& operator=(const Command&) = delete;

	Command(Command&&) noexcept = default;
	Command& operator=(Command&&) noexcept = default;
};