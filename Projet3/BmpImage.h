
#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct BmpImage
{
    int32_t width = 0;
    int32_t height = 0;
    uint32_t pixelOffset = 0;

    std::vector<unsigned char> header;
    std::vector<unsigned char> pixels;
    std::vector<unsigned char> paddings;
};

bool loadImage(const char *fileName, BmpImage &image, std::string &errorMessage);

bool saveImage(const char *fileName, const BmpImage &image, std::string &errorMessage);
