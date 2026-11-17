
#include "BmpImage.h"

#include <cstdint>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>

// BMP constants
const uint32_t BMP_HEADER_SIZE = 40;
const uint16_t BITS_PER_PIXEL = 24;
const uint32_t NO_COMPRESSION = 0;
const int BYTES_PER_PIXEL = 3;
const int BMP_DATA_START = 54;
const int BYTE_ALIGNMENT = 4;

bool loadImage(const char *fileName, BmpImage &image, std::string &errorMessage)
{
    errorMessage.clear();

    FILE *file = fopen(fileName, "rb");

    if (file == nullptr)
    {
        errorMessage = "Cannot open file.";
        return false;
    }

    char signature[2];

    if (fread(signature, 1, 2, file) != 2 || signature[0] != 'B' || signature[1] != 'M')
    {
        errorMessage = "Invalid BMP file.";
        fclose(file);
        return false;
    }

    int32_t width = 0;
    int32_t height = 0;
    uint16_t bitsPerPixel = 0;
    uint32_t compression = 0;
    uint32_t pixelOffset = 0;
    uint32_t dibSize = 0;

    bool readOk =
        fseek(file, 10, SEEK_SET) == 0 && fread(&pixelOffset, 4, 1, file) == 1 &&
        fseek(file, 14, SEEK_SET) == 0 && fread(&dibSize, 4, 1, file) == 1 &&
        fseek(file, 18, SEEK_SET) == 0 && fread(&width, 4, 1, file) == 1 &&
        fread(&height, 4, 1, file) == 1 && fseek(file, 28, SEEK_SET) == 0 &&
        fread(&bitsPerPixel, 2, 1, file) == 1 && fread(&compression, 4, 1, file) == 1;

    if (!readOk)
    {
        errorMessage = "Cannot read BMP header.";
        fclose(file);
        return false;
    }

    if (width <= 0 || height <= 0 || bitsPerPixel != BITS_PER_PIXEL ||
        compression != NO_COMPRESSION || dibSize != BMP_HEADER_SIZE ||
        pixelOffset < BMP_DATA_START)
    {
        errorMessage = "Unsupported BMP format.";
        fclose(file);
        return false;
    }

    if (width > std::numeric_limits<int>::max() / BYTES_PER_PIXEL)
    {
        errorMessage = "Image is too wide.";
        fclose(file);
        return false;
    }

    int rowBytes = width * BYTES_PER_PIXEL;
    int padding = (BYTE_ALIGNMENT - (rowBytes % BYTE_ALIGNMENT)) % BYTE_ALIGNMENT;

    if (fseek(file, 0, SEEK_END) != 0)
    {
        errorMessage = "Cannot read file size.";
        fclose(file);
        return false;
    }

    long fileSize = ftell(file);

    if (fileSize < 0)
    {
        errorMessage = "Cannot read file size.";
        fclose(file);
        return false;
    }

    uint64_t rowSize = uint64_t(rowBytes) + padding;
    uint64_t requiredSize = uint64_t(pixelOffset) + rowSize * height;

    if (requiredSize > uint64_t(fileSize))
    {
        errorMessage = "BMP file is truncated.";
        fclose(file);
        return false;
    }

    if (pixelOffset > uint64_t(std::numeric_limits<long>::max()))
    {
        errorMessage = "BMP pixel offset is too large.";
        fclose(file);
        return false;
    }

    BmpImage result;
    result.width = width;
    result.height = height;
    result.pixelOffset = pixelOffset;

    result.header.resize(pixelOffset);

    if (fseek(file, 0, SEEK_SET) != 0 ||
        fread(result.header.data(), 1, pixelOffset, file) != pixelOffset)
    {
        errorMessage = "Cannot read full BMP header.";
        fclose(file);
        return false;
    }

    if (fseek(file, static_cast<long>(pixelOffset), SEEK_SET) != 0)
    {
        errorMessage = "Cannot reach BMP pixels.";
        fclose(file);
        return false;
    }

    // Read BGR pixels
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            unsigned char pixel[3];

            if (fread(pixel, 1, BYTES_PER_PIXEL, file) != BYTES_PER_PIXEL)
            {
                errorMessage = "Cannot read pixel.";
                fclose(file);
                return false;
            }

            result.pixels.push_back(pixel[0]);
            result.pixels.push_back(pixel[1]);
            result.pixels.push_back(pixel[2]);
        }

        // Keep original padding bytes
        for (int p = 0; p < padding; ++p)
        {
            unsigned char byte = 0;

            if (fread(&byte, 1, 1, file) != 1)
            {
                errorMessage = "Cannot read padding.";
                fclose(file);
                return false;
            }

            result.paddings.push_back(byte);
        }
    }

    if (fclose(file) != 0)
    {
        errorMessage = "Cannot close file.";
        return false;
    }

    image = result;
    return true;
}

bool saveImage(const char *fileName, const BmpImage &image, std::string &errorMessage)
{
    errorMessage.clear();

    if (image.width <= 0 || image.height <= 0 ||
        image.width > std::numeric_limits<int>::max() / BYTES_PER_PIXEL)
    {
        errorMessage = "Invalid image dimensions.";
        return false;
    }

    int rowBytes = image.width * BYTES_PER_PIXEL;
    int padding = (BYTE_ALIGNMENT - (rowBytes % BYTE_ALIGNMENT)) % BYTE_ALIGNMENT;

    uint64_t pixelCount = uint64_t(image.width) * image.height * BYTES_PER_PIXEL;

    uint64_t paddingCount = uint64_t(image.height) * padding;

    if (image.pixels.size() != pixelCount || image.paddings.size() != paddingCount ||
        image.header.size() != image.pixelOffset)
    {
        errorMessage = "Invalid BMP data.";
        return false;
    }

    // Do not overwrite an existing file
    FILE *destination = fopen(fileName, "wbx");

    if (destination == nullptr)
    {
        errorMessage = "Destination already exists or is inaccessible.";
        return false;
    }

    if (fwrite(image.header.data(), 1, image.header.size(), destination) !=
        image.header.size())
    {
        errorMessage = "Cannot write BMP header.";
        fclose(destination);
        return false;
    }

    size_t index = 0;
    size_t paddingIndex = 0;

    for (int y = 0; y < image.height; ++y)
    {
        for (int x = 0; x < image.width; ++x)
        {
            if (fwrite(&image.pixels[index], 1, BYTES_PER_PIXEL, destination) !=
                BYTES_PER_PIXEL)
            {
                errorMessage = "Cannot write pixel.";
                fclose(destination);
                return false;
            }

            index += BYTES_PER_PIXEL;
        }

        // Write original padding
        for (int p = 0; p < padding; ++p)
        {
            if (fwrite(&image.paddings[paddingIndex], 1, 1, destination) != 1)
            {
                errorMessage = "Cannot write padding.";
                fclose(destination);
                return false;
            }

            ++paddingIndex;
        }
    }

    if (fclose(destination) != 0)
    {
        errorMessage = "Cannot close destination.";
        return false;
    }

    return true;
}
