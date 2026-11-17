
#include "Steganography.h"

#include <cstdint>
#include <limits>
#include <string>

// Hide one byte in 8 pixel components
static void hideByte(BmpImage &image, unsigned char byte, size_t &index)
{
    for (int i = 0; i < BITS_PER_BYTE; ++i)
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
        errorMessage = "Image is too small.";
        return false;
    }

    size_t capacity = image.pixels.size() / BITS_PER_BYTE - METADATA_SIZE;

    if (message.size() > capacity)
    {
        errorMessage = "Message is too large for this image.";
        return false;
    }

    if (message.size() > std::numeric_limits<uint32_t>::max())
    {
        errorMessage = "Message length is too large.";
        return false;
    }

    size_t index = 0;

    // Hide STEG signature
    for (int i = 0; i < SIGNATURE_SIZE; ++i)
    {
        hideByte(image, STEG_SIGNATURE[i], index);
    }

    // Hide encryption flag
    unsigned char encryptionFlag = encrypted ? 1 : 0;
    hideByte(image, encryptionFlag, index);

    // Hide message length (little-endian)
    uint32_t messageLength = static_cast<uint32_t>(message.size());

    for (int byteIndex = 0; byteIndex < LENGTH_SIZE; ++byteIndex)
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
