#include <string>

enum class CommandType
{
	None,
	Connect,
	Send
};

struct Command
{
	CommandType Type = CommandType::None;
	std::string Data;

	Command() = {}
	Command(CommandType type, const std::string& data) : Type(type), Data(data) {}
};