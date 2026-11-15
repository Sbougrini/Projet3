#include "CommandLine.h"

#include <iostream>

bool CommandLine::verifyCommandLine(int argc, char* argv[], Command& command, std::string& error)
{
	if (argc < 2)
	{
		command.mode = Mode::HELP;
		return true;
	}
	std::string commandName = argv[1];
	if (commandName == "help") 
	{
		command.mode = Mode::HELP;
		return true;
	}
	error = "Commande inconnue : " + commandName;
	return false;
}

void CommandLine::printHelp()
{
	std::cout << "Commandes : " << std::endl;
	std::cout << "help :	" << std::endl;
	std::cout << "embed :	" << std::endl;
	std::cout << "extract :	" << std::endl;
}