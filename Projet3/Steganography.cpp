
#include "Steganography.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>

// Hide one byte in 8 pixel components
static void hideByte(BmpImage &image, unsigned char byte, size_t &index)
{
    for (std::size_t i = 0; i < BITS_PER_BYTE; ++i)
    {
        int bit = (byte >> (7 - i)) & 1;

        image.pixels[index] = (image.pixels[index] & 0xFE) | bit;

        ++index;
    }
}

bool hideMessage(BmpImage &image, const std::string &message, bool encrypted,
                 std::string &errorMessage)
{
    errorMessage.clear();

    if (image.pixels.size() < METADATA_SIZE * BITS_PER_BYTE)
    {
        errorMessage = "L'image est trop petite.";
        return false;
    }

    size_t capacity = image.pixels.size() / BITS_PER_BYTE - METADATA_SIZE;

    if (message.size() > capacity)
    {
        errorMessage = "Le message est trop grand pour cette image.";
        return false;
    }

    if (message.size() > std::numeric_limits<uint32_t>::max())
    {
        errorMessage = "Le message est trop long.";
        return false;
    }

    size_t index = 0;

    // Hide STEG signature
    for (std::size_t i = 0; i < SIGNATURE_SIZE; ++i)
    {
        hideByte(image, STEG_SIGNATURE[i], index);
    }

    // Hide encryption flag
    unsigned char encryptionFlag = encrypted ? 1 : 0;
    hideByte(image, encryptionFlag, index);

    // Hide message length (little-endian)
    uint32_t messageLength = static_cast<uint32_t>(message.size());

    for (std::size_t byteIndex = 0; byteIndex < LENGTH_SIZE; ++byteIndex)
    {
        unsigned char value = (messageLength >> (byteIndex * BITS_PER_BYTE)) & 0xFF;

        hideByte(image, value, index);
    }

    // Hide message
    for (unsigned char character : message)
    {
        hideByte(image, character, index);
    }

    return true;
}

static unsigned char readHiddenByte(const std::vector<unsigned char> &imageData,
                                    std::size_t hiddenByteIndex)
{
    assert((hiddenByteIndex + 1) * BITS_PER_BYTE <= imageData.size());
    unsigned char byte = 0;
    for (std::size_t i = 0; i < BITS_PER_BYTE; ++i)
    {
        std::size_t componentIndex = BITS_PER_BYTE * hiddenByteIndex + i;
        unsigned char bit = 0;
        bit = imageData[componentIndex] & 1;
        byte |= (bit << (7 - i));
    }
    return byte;
}

static bool checkSignature(const std::vector<unsigned char> &imageData,
                           std::string &error)
{
    for (std::size_t i = 0; i < SIGNATURE_SIZE; ++i)
    {
        unsigned char byte = readHiddenByte(imageData, i);
        if (byte != static_cast<unsigned char>(STEG_SIGNATURE[i]))
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
        unsigned char byte = readHiddenByte(imageData, SIGNATURE_SIZE + 1 + i);
        length |= (static_cast<std::uint32_t>(byte) << (BITS_PER_BYTE * i));
    }
    return length;
}

bool extractMessage(const std::vector<unsigned char> &imageData, std::string &message,
                    bool &isEncrypted, std::string &error)
{
    if (imageData.size() < METADATA_SIZE * BITS_PER_BYTE)
    {
        error = "L'image est trop petite pour contenir un message cache.";
        return false;
    }
    if (!checkSignature(imageData, error))
    {
        return false;
    }
    unsigned char flag = readHiddenByte(imageData, SIGNATURE_SIZE);
    if (flag > 1)
    {
        error = "Drapeau de chiffrement invalide";
        return false;
    }
    isEncrypted = flag == 1;
    std::uint32_t hiddenLength = readHiddenLength(imageData);
    if (hiddenLength > (imageData.size() / BITS_PER_BYTE) - METADATA_SIZE)
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