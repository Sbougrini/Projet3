#include "CommandLine.h"
#include "bmpImage.h"
#include "Steganography.h"

#include <iostream>
#include <string>


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
    std::string errorMessage;
    std::string message = "Bonjour";
    bool encrypted = false;

    // Charger l'image
    if (!loadImage("images/Image.bmp", image, errorMessage))
    {
        std::cout << errorMessage << '\n';
        return 1;
    }

    // Afficher les informations de l'image
    std::cout << "Width : " << image.width << '\n';
    std::cout << "Height : " << image.height << '\n';
    std::cout << "Pixel bytes : " << image.pixels.size() << '\n';

    // Cacher le message et les metadonnees
    if (!hideMessage(image, message, encrypted, errorMessage))
    {
        std::cout << errorMessage << '\n';
        return 1;
    }

    // Sauvegarder l'image modifiee
    if (!saveImage("images/Image_cachee.bmp", image, errorMessage))
    {
        std::cout << errorMessage << '\n';
        return 1;
    }

    std::cout << "Message hidden : " << message << '\n';
    std::cout << "Image saved successfully.\n";

    return 0;

    return 0;
}




   

  

