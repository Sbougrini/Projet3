#include "CommandLine.h"

#include <iostream>

bool CommandLine::verifyCommandLine(int argc, char *argv[], Command &command,
                                    std::string &error)
{
    if (argc < 2)
    {
        return verifyHelp(argc, argv, command, error);
    }
    std::string commandName = argv[1];
    if (commandName == "help")
    {
        return verifyHelp(argc, argv, command, error);
    }
    if (commandName == "extract")
    {
        return verifyExtract(argc, argv, command, error);
    }
    if (commandName == "embed")
    {
        return verifyEmbed(argc, argv, command, error);
    }
    error = "Commande inconnue : " + commandName;
    return false;
}

bool CommandLine::verifyKey(int argc, char *argv[], Command &command, std::string &error,
                            int keyIndex)
{
    if (keyIndex + 1 >= argc)
    {
        error = "--key doit etre suivi d'une cle.";
        return false;
    }
    std::string option = argv[keyIndex];
    if (option != "--key")
    {
        error = "Option inconnue : " + option;
        return false;
    }

    std::string key = argv[keyIndex + 1];
    if (key.empty())
    {
        error = "La cle ne doit pas etre vide.";
        return false;
    }
    command.key = key;
    return true;
}

bool CommandLine::verifyExtract(int argc, char *argv[], Command &command,
                                std::string &error)
{
    if (argc != 3 && argc != 5)
    {
        error = "Nombre d'arguments incorrect pour extract.";
        return false;
    }
    command.mode = Mode::EXTRACT;
    command.source = argv[2];
    if (argc == 5)
    {
        return verifyKey(argc, argv, command, error, 3);
    }
    return true;
}

bool CommandLine::verifyEmbed(int argc, char *argv[], Command &command,
                              std::string &error)
{
    if (argc != 5 && argc != 7)
    {
        error = "Nombre d'arguments incorrect pour embed.";
        return false;
    }
    command.mode = Mode::EMBED;
    command.source = argv[2];
    command.destination = argv[3];
    command.message = argv[4];
    if (command.message.empty())
    {
        error = "Le message a cacher ne doit pas etre vide.";
        return false;
    }
    if (argc == 7)
    {
        return verifyKey(argc, argv, command, error, 5);
    }

    return true;
}

bool CommandLine::verifyHelp(int argc, char *[], Command &command, std::string &error)
{
    if (argc != 1 && argc != 2)
    {
        error = "help ne prend pas d'argument.";
        return false;
    }
    command.mode = Mode::HELP;
    return true;
}

void CommandLine::printHelp()
{
    std::cout << "Commandes : " << std::endl;
    std::cout << "help : steg help" << std::endl;
    std::cout << "embed :	steg embed <source.bmp> <destination.bmp> \"<message>\" "
                 "[--key \"<cle>\" ]"
              << std::endl;
    std::cout << "extract : steg extract <source.bmp> [--key \"<cle>\" ]" << std::endl;
    std::cout << "\nExemples : " << std::endl;
    std::cout << "steg embed \"images/ma voiture.bmp\" \"images/voiture perso.bmp\" "
                 "\"rdv a 16h\""
              << std::endl;
    std::cout << "steg embed \"images/ma voiture.bmp\" \"images/voiture perso.bmp\" "
                 "\"rdv a 16h\" --key \"ma cle\" "
              << std::endl;
    std::cout << "steg extract \"images/voiture perso.bmp\" --key \"ma cle\" "
              << std::endl;
}