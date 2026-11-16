#include "Steganography.h"

#include <iostream>
#include <string>

void cacherOctet(BmpImage &image, unsigned char octet, int &index)
{
    for (int i = 0; i < 8; i++)
    {
        int bit = (octet >> (7 - i)) & 1;
        image.pixels[index] = (image.pixels[index] & 0xFE) | bit;
        index++;
    }
}

bool cacherMessage(BmpImage &image, const std::string &message)
{
    if (image.pixels.size() < 72)
    {
        return false;
    }

    int capacite = static_cast<int>(image.pixels.size() / 8) - 9;

    if (message.size() > capacite)
    {
        std::cout << "Image trop petite.\n";
        return false;
    }

    std::string signature = "STEG";

    if ((9 + message.size()) * 8 > image.pixels.size())
    {
        return false;
    }

    int index = 0;

    for (char c : signature)
    {
        cacherOctet(image, c, index);
    }

    unsigned char chiffrement = 0;

    cacherOctet(image, chiffrement, index);

    uint32_t longueur = message.size();
    
    for (int octet = 0; octet < 4; octet++)
    {
        unsigned char valeur = (longueur >> (octet * 8)) & 0xFF;
        cacherOctet(image, valeur, index);
    }

    for (char c : message)
    {
        cacherOctet (image, c, index);
    }

    return true;
}