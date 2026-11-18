#include "CommandLine.h"

#include <iostream>

bool CommandLine::verifyCommandLine(int argc, char *argv[], Command &command,
                                    std::string &error)
{
    std::vector<std::string> arguments(argv, argv + argc);
    if (arguments.size() < HELP_ARGUMENT_COUNT)
    {
        return verifyHelp(arguments, command, error);
    }
    std::string commandName = arguments[1];
    if (commandName == "help")
    {
        return verifyHelp(arguments, command, error);
    }
    if (commandName == "extract")
    {
        return verifyExtract(arguments, command, error);
    }
    if (commandName == "embed")
    {
        return verifyEmbed(arguments, command, error);
    }
    error = "Commande inconnue : " + commandName;
    return false;
}

bool CommandLine::verifyKey(const std::vector<std::string> &arguments, Command &command,
                            std::string &error, std::size_t keyIndex)
{
    if (keyIndex + 1 >= arguments.size())
    {
        error = "--key doit etre suivi d'une cle.";
        return false;
    }
    if (arguments[keyIndex] != KEY_OPTION)
    {
        error = "Option inconnue : " + arguments[keyIndex];
        return false;
    }
    if (arguments[keyIndex + 1].empty())
    {
        error = "La cle ne doit pas etre vide.";
        return false;
    }
    command.key = arguments[keyIndex + 1];
    return true;
}

bool CommandLine::verifyExtract(const std::vector<std::string> &arguments,
                                Command &command, std::string &error)
{
    if (arguments.size() != EXTRACT_ARGUMENT_COUNT &&
        arguments.size() != EXTRACT_ARGUMENT_COUNT + KEY_OPTION_ARGUMENT_COUNT)
    {
        error = "Nombre d'arguments incorrect pour extract.";
        return false;
    }
    command.mode = Mode::EXTRACT;
    command.source = arguments[2];
    if (arguments.size() == EXTRACT_ARGUMENT_COUNT + KEY_OPTION_ARGUMENT_COUNT)
    {
        return verifyKey(arguments, command, error, EXTRACT_ARGUMENT_COUNT);
    }
    return true;
}

bool CommandLine::verifyEmbed(const std::vector<std::string> &arguments, Command &command,
                              std::string &error)
{
    if (arguments.size() != EMBED_ARGUMENT_COUNT &&
        arguments.size() != EMBED_ARGUMENT_COUNT + KEY_OPTION_ARGUMENT_COUNT)
    {
        error = "Nombre d'arguments incorrect pour embed.";
        return false;
    }
    command.mode = Mode::EMBED;
    command.source = arguments[2];
    command.destination = arguments[3];
    command.message = arguments[4];
    if (command.message.empty())
    {
        error = "Le message a cacher ne doit pas etre vide.";
        return false;
    }
    if (arguments.size() == EMBED_ARGUMENT_COUNT + KEY_OPTION_ARGUMENT_COUNT)
    {
        return verifyKey(arguments, command, error, EMBED_ARGUMENT_COUNT);
    }
    return true;
}

bool CommandLine::verifyHelp(const std::vector<std::string> &arguments, Command &command,
                             std::string &error)
{
    if (arguments.size() != NO_COMMAND_ARGUMENT_COUNT &&
        arguments.size() != HELP_ARGUMENT_COUNT)
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