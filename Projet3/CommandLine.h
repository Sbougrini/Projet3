#pragma once

#include <string>

enum class Mode
{
	HELP = 0,
	EMBED,
	EXTRACT
};

struct Command
{
	Mode mode = Mode::HELP;
	std::string source;
	std::string destination;
	std::string message;
	std::string key;
};

class CommandLine
{
public:
	CommandLine() = delete;
	static bool verifyCommandLine(int argc, char* argv[], Command& command, std::string& error);
	static void printHelp();

private:
	static bool verifyHelp(int argc, char* argv[], Command& command, std::string& error);
	static bool verifyEmbed(int argc, char* argv[], Command& command, std::string& error);
	static bool verifyExtract(int argc, char* argv[], Command& command, std::string& error);
	static bool verifyKey(int argc, char* argv[], Command& command, std::string& error, int keyIndex);
};