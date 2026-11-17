#include "Steganography.h"

#include <cassert>
#include <cstdint>

static unsigned char readHiddenByte(const std::vector<unsigned char> &imageData,
                                    std::size_t hiddenByteIndex)
{
    assert((hiddenByteIndex + 1) * 8 <= imageData.size());
    unsigned char byte = 0;
    for (std::size_t i = 0; i < 8; ++i)
    {
        std::size_t componentIndex = 8 * hiddenByteIndex + i;
        unsigned char bit = 0;
        bit = imageData[componentIndex] & 1;
        byte |= (bit << (7 - i));
    }
    return byte;
}

static bool checkSignature(const std::vector<unsigned char> &imageData,
                           std::string &error)
{
    for (std::size_t i = 0; i < SIGNATURE.size(); ++i)
    {
        unsigned char byte = readHiddenByte(imageData, i);
        if (byte != static_cast<unsigned char>(SIGNATURE[i]))
        {
            error = "Aucun message cache dans l'image.";
            return false;
        }
    }
    return true;
}

static std::uint32_t readHiddenLength(const std::vector<unsigned char> &imageData)
{
    std::uint32_t length = 0;
    for (std::size_t i = 0; i < sizeof(length); ++i)
    {
        unsigned char byte = readHiddenByte(imageData, SIGNATURE.size() + 1 + i);
        length |= (static_cast<std::uint32_t>(byte) << (8 * i));
    }
    return length;
}

bool extractMessage(const std::vector<unsigned char> &imageData, std::string &message,
                    bool &isEncrypted, std::string &error)
{
    if (imageData.size() < METADATA_SIZE * 8)
    {
        error = "L'image est trop petite pour contenir un message cache.";
        return false;
    }
    if (!checkSignature(imageData, error))
    {
        return false;
    }
    unsigned char flag = readHiddenByte(imageData, SIGNATURE.size());
    if (flag > 1)
    {
        error = "Drapeau de chiffrement invalide";
        return false;
    }
    isEncrypted = flag == 1;
    std::uint32_t hiddenLength = readHiddenLength(imageData);
    if (hiddenLength > (imageData.size() / 8) - METADATA_SIZE)
    {
        error = "La longueur du message cache est invalide.";
        return false;
    }
    message.clear();
    for (std::size_t i = 0; i < hiddenLength; ++i)
    {
        unsigned char byte = readHiddenByte(imageData, METADATA_SIZE + i);
        message.push_back(static_cast<char>(byte));
    }
    return true;
}