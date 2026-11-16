#include "Steganography.h"

#include <iostream>
#include <string>

bool cacherMessage(BmpImage &image, const std::string &message)
{
    std::string signature = "STEG";

    if ((9 + message.size()) * 8 > image.pixels.size())
    {
        return false;
    }

    int index = 0;

    for (char c : signature)
    {
        for (int i = 0; i < 8; i++)
        {
            int bit = (c >> (7 - i)) & 1;
            
            image.pixels[index] = (image.pixels[index] & 0xFE) | bit;

            index++;
        }
    }

    unsigned char chiffrement = 0;

    for (int i = 0; i < 8; i++)
    {
        int bit = (chiffrement >> (7 - i)) & 1;
        image.pixels[index] = (image.pixels[index] & 0xFE) | bit;
        index++;
    }

    uint32_t longueur = message.size();
    
    for (int octet = 0; octet < 4; octet++)
    {
        for (int i = 0; i < 8; i++)
        {
            int bit = (longueur >> (octet * 8 + (7 - i))) & 1;

            image.pixels[index] = (image.pixels[index] & 0xFE) | bit;

            index++;
        }
    }

    for (char c : message)
    {
        for (int i = 0; i < 8; i++)
        {
            int bit = (c >> (7 - i)) & 1;
            image.pixels[index] = (image.pixels[index] & 0xFE) | bit;
            index++;
        }
    }

    return true;
}