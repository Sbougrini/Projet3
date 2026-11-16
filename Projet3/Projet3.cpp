#include "CommandLine.h"
#include <iostream>
#include "bmpImage.h"

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

    std::cout << "[test] mode : " << static_cast<int>(command.mode) << std::endl;
    std::cout << "[test] source : [" << command.source << "]" << std::endl;
    std::cout << "[test] destination : [" << command.destination << "]" << std::endl;
    std::cout << "[test] message : [" << command.message << "]" << std::endl;
    std::cout << "[test] cle : [" << command.key << "]" << std::endl;

    if (command.mode == Mode::HELP)
    {
        CommandLine::printHelp();
    }

  BmpImage image;

    // Charger l'image
    if (!chargerImage("images/Image.bmp", image))
    {
        return 1;
    }

    // Message a cacher
    std::string message = "Bonjour";

    // Verification de la capacite
    int bitsNecessaires = message.size() * 8;

    if (bitsNecessaires > image.pixels.size())
    {
        std::cout << "Image trop petite.\n";
        return 1;
    }

    int index = 0;

    // Cacher chaque caractere
    for (int j = 0; j < message.size(); j++)
    {
        unsigned char caractere = message[j];

        for (int i = 0; i < 8; i++)
        {
            int bit = (caractere >> (7 - i)) & 1;

            image.pixels[index] = (image.pixels[index] & 0xFE) | bit;

            index++;
        }
    }

    // Sauvegarder l'image
    if (!sauvegarderImage("images/Image_cachee.bmp", image))
    {
        return 1;
    }

    std::cout << "Message cache : " << message << '\n';


    return 0;
}




   

  

