#include <iostream>
#include "CommandLine.h"

int main(int argc, char* argv[])
{
    Command commandName;
    std::string error;
    if (!CommandLine::verifyCommandLine(argc, argv, commandName, error))
    {
        std::cout << "Erreur : " << error << std::endl;
        CommandLine::printHelp();
        return 1;
    }
    if (commandName.mode == Mode::HELP)
    {
        CommandLine::printHelp();
    }
    return 0;
}