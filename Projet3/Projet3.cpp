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
    std::string original = "Salut a tous";
    std::string encrypted = Xor::encryptDecrypt(original, "ma cle");
    std::string decrypted = Xor::encryptDecrypt(encrypted, "ma cle");
    std::string wrongKey = Xor::encryptDecrypt(encrypted, "autre");
    std::cout << "[test] original    : [" << original << "]" << std::endl;
    std::cout << "[test] chiffre     : [" << encrypted << "]" << std::endl;
    std::cout << "[test] dechiffre   : [" << decrypted << "]" << std::endl;
    std::cout << "[test] mauvaise cle: [" << wrongKey << "]" << std::endl;
    std::cout << "[test] identique ? " << (decrypted == original ? "oui" : "non")
              << std::endl;
    if (command.mode == Mode::HELP)
    {
        CommandLine::printHelp();
    }
    return 0;
}