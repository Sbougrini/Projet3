
#pragma once

#include <cstdint>
#include <vector>

struct BmpImage
{
    int32_t largeur = 0;
    int32_t hauteur = 0;
    uint32_t offsetPixels = 0;

    std::vector<unsigned char> entete;
    std::vector<unsigned char> pixels;
};

bool chargerImage(const char * nomFichier, BmpImage& image);

bool sauvegarderImage(const char *nomFichier, const BmpImage &image);
