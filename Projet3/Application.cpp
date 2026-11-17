#include "Application.h"

#include <iostream>
#include <string>

#include "BmpImage.h"
#include "CommandLine.h"
#include "Steganography.h"
#include "Xor.h"

int Application::run(int argc, char *argv[])
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
    if (command.mode == Mode::EMBED)
    {
        return runEmbed(command);
    }
    if (command.mode == Mode::EXTRACT)
    {
        return runExtract(command);
    }
    return 0;
}

int Application::runEmbed(const Command &command)
{
    BmpImage image = {};
    std::string error;
    if (!loadImage(command.source.c_str(), image, error))
    {
        std::cout << "Erreur : " << error << std::endl;
        return 1;
    }
    bool isKeyFilled = !command.key.empty();
    std::string message = command.message;
    if (isKeyFilled)
    {
        message = Xor::encryptDecrypt(message, command.key);
    }
    if (!hideMessage(image, message, isKeyFilled, error))
    {
        std::cout << "Erreur : " << error << std::endl;
        return 1;
    }
    if (!saveImage(command.destination.c_str(), image, error))
    {
        std::cout << "Erreur : " << error << std::endl;
        return 1;
    } 
    std::cout << "Message cache avec succes dans l'image : " << command.destination << std::endl;
    return 0;
}

int Application::runExtract(const Command &command)
{
    BmpImage image = {};
    std::string error;
    std::string message;
    bool isEncrypted = false;
    if (!loadImage(command.source.c_str(), image, error))
    {
        std::cout << "Erreur : " << error << std::endl;
        return 1;
    }
    if (!extractMessage(image.pixels, message, isEncrypted, error))
    {
        std::cout << "Erreur : " << error << std::endl;
        return 1;
    }
    if (isEncrypted && command.key.empty())
    {
        std::cout << "Erreur : ce message est chiffre, utilisez --key" << std::endl;
        return 1;
    }
    if (!isEncrypted && !command.key.empty())
    {
        std::cout << "Attention : ce message n'est pas chiffre, la cle est ignoree"
                  << std::endl;
    }
    if (isEncrypted)
    {
        message = Xor::encryptDecrypt(message, command.key);
    }
    std::cout << "Message cache : " << message << std::endl;
    return 0;
}