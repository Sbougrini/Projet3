
#include "BmpImage.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

int ouvrirFichier(const char *nomFichier)
{
    FILE *fichier = nullptr;
    fopen_s(&fichier, nomFichier, "rb");

    if (fichier == nullptr)
    {
        std::cout << "Erreur ouverture fichier.\n";
        return 1;
    }

    // Verification de la signature BMP
    char signature[2];

    if (fread(signature, sizeof(char), 2, fichier) != 2)
    {
        std::cout << "Erreur lecture signature.\n";
        fclose(fichier);
        return 1;
    }

    if (signature[0] != 'B' || signature[1] != 'M')
    {
        std::cout << "Format BMP invalide.\n";
        fclose(fichier);
        return 1;
    }

    // Lecture de la largeur
    int32_t largeur = 0;

    if (fseek(fichier, 18, SEEK_SET) != 0 ||
        fread(&largeur, sizeof(int32_t), 1, fichier) != 1)
    {
        std::cout << "Erreur lecture largeur.\n";
        fclose(fichier);
        return 1;
    }

    // Lecture de la hauteur
    int32_t hauteur = 0;

    if (fseek(fichier, 22, SEEK_SET) != 0 ||
        fread(&hauteur, sizeof(int32_t), 1, fichier) != 1)
    {
        std::cout << "Erreur lecture hauteur.\n";
        fclose(fichier);
        return 1;
    }

    if (largeur <= 0 || hauteur <= 0)
    {
        std::cout << "Dimensions non prises en charge.\n";
        fclose(fichier);
        return 1;
    }

    // Verification des 24 bits par pixel
    uint16_t bitsParPixel = 0;

    if (fseek(fichier, 28, SEEK_SET) != 0 ||
        fread(&bitsParPixel, sizeof(uint16_t), 1, fichier) != 1)
    {
        std::cout << "Erreur lecture bits par pixel.\n";
        fclose(fichier);
        return 1;
    }

    if (bitsParPixel != 24)
    {
        std::cout << "Le BMP doit etre en 24 bits.\n";
        fclose(fichier);
        return 1;
    }

    // Verification de la compression
    uint32_t compression = 0;

    if (fseek(fichier, 30, SEEK_SET) != 0 ||
        fread(&compression, sizeof(uint32_t), 1, fichier) != 1)
    {
        std::cout << "Erreur lecture compression.\n";
        fclose(fichier);
        return 1;
    }

    if (compression != 0)
    {
        std::cout << "Le BMP doit etre non compresse.\n";
        fclose(fichier);
        return 1;
    }

    // Lecture de la position des pixels
    uint32_t offsetPixels = 0;

    if (fseek(fichier, 10, SEEK_SET) != 0 ||
        fread(&offsetPixels, sizeof(uint32_t), 1, fichier) != 1)
    {
        std::cout << "Erreur lecture offset pixels.\n";
        fclose(fichier);
        return 1;
    }

    // Calcul du padding
    int octetsLigne = largeur * 3;
    int padding = (4 - (octetsLigne % 4)) % 4;
    int tailleLigne = octetsLigne + padding;

    std::cout << "Largeur : " << largeur << '\n';
    std::cout << "Hauteur : " << hauteur << '\n';
    std::cout << "Bits par pixel : " << bitsParPixel << '\n';
    std::cout << "Compression : " << compression << '\n';
    std::cout << "Offset pixels : " << offsetPixels << '\n';
    std::cout << "Taille ligne : " << tailleLigne << '\n';
    std::cout << "Padding : " << padding << '\n';

    // Placement au debut des pixels
    if (fseek(fichier, offsetPixels, SEEK_SET) != 0)
    {
        std::cout << "Erreur positionnement pixels.\n";
        fclose(fichier);
        return 1;
    }

    // Stockage des composantes BGR
    std::vector<unsigned char> pixels;

    for (int y = 0; y < hauteur; y++)
    {
        for (int x = 0; x < largeur; x++)
        {
            unsigned char pixel[3];

            if (fread(pixel, sizeof(unsigned char), 3, fichier) != 3)
            {
                std::cout << "Erreur lecture pixel.\n";
                fclose(fichier);
                return 1;
            }

            pixels.push_back(pixel[0]);
            pixels.push_back(pixel[1]);
            pixels.push_back(pixel[2]);
        }

        // Ignorer les octets de padding
        if (fseek(fichier, padding, SEEK_CUR) != 0)
        {
            std::cout << "Erreur lecture padding.\n";
            fclose(fichier);
            return 1;
        }
    }

    std::cout << "Octets stockes : " << pixels.size() << '\n';

    // Test : cacher les 8 bits du caractere A
    unsigned char caractere = 'A';
    int index = 0;

    if (pixels.size() >= 8)
    {
        for (int i = 7; i >= 0; i--)
        {
            int bit = (caractere >> i) & 1;

            pixels[index] = (pixels[index] & 254) | bit;

            index++;
        }

        std::cout << "Caractere A cache dans le vector.\n";
    }

    fclose(fichier);
    return 0;
}
