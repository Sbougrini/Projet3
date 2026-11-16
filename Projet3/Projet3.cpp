#include "CommandLine.h"
#include "Xor.h"

#include <iostream>

int main(int argc, char *argv[])
{
    Command command;
    std::string error;
    if (!CommandLine::verifyCommandLine(argc, argv, command, error))
    {
        std::cout << "Erreur : " << error << std::endl;
        CommandLine::printHelp();
        return 1;
    }
    if (command.mode == Mode::HELP)
    {
        CommandLine::printHelp();
    }
    return 0;
}