
#include "BmpImage.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

bool chargerImage(const char *nomFichier, BmpImage &image)
{
    FILE *fichier = nullptr;
    fopen_s(&fichier, nomFichier, "rb");

    if (fichier == nullptr)
    {
        std::cout << "Erreur ouverture fichier.\n";
        return false;
    }

    // Verification de la signature BMP
    char signature[2];

    if (fread(signature, 1, 2, fichier) != 2 || signature[0] != 'B' ||
        signature[1] != 'M')
    {
        std::cout << "Fichier BMP invalide.\n";
        fclose(fichier);
        return false;
    }

    int32_t largeur = 0;
    int32_t hauteur = 0;
    uint16_t bitsParPixel = 0;
    uint32_t compression = 0;
    uint32_t offsetPixels = 0;
    uint32_t tailleDib = 0;

    // Lecture des informations BMP
    bool lectureOk =
        fseek(fichier, 10, SEEK_SET) == 0 && fread(&offsetPixels, 4, 1, fichier) == 1 &&
        fseek(fichier, 14, SEEK_SET) == 0 && fread(&tailleDib, 4, 1, fichier) == 1 &&
        fseek(fichier, 18, SEEK_SET) == 0 && fread(&largeur, 4, 1, fichier) == 1 &&
        fread(&hauteur, 4, 1, fichier) == 1 && fseek(fichier, 28, SEEK_SET) == 0 &&
        fread(&bitsParPixel, 2, 1, fichier) == 1 &&
        fread(&compression, 4, 1, fichier) == 1;

    if (!lectureOk)
    {
        std::cout << "Erreur lecture en-tete.\n";
        fclose(fichier);
        return false;
    }

    // Verification du format
    if (largeur <= 0 || hauteur <= 0 || bitsParPixel != 24 || compression != 0 ||
        tailleDib < 40 || offsetPixels < 14 + tailleDib)
    {
        std::cout << "Format BMP non pris en charge.\n";
        fclose(fichier);
        return false;
    }

    // Calcul du padding
    int octetsLigne = largeur * 3;
    int padding = (4 - (octetsLigne % 4)) % 4;

    // Enregistrer les informations dans la structure
    image.largeur = largeur;
    image.hauteur = hauteur;
    image.offsetPixels = offsetPixels;

    // Lecture de l'en-tete complet
    image.entete.resize(offsetPixels);

    fseek(fichier, 0, SEEK_SET);

    if (fread(image.entete.data(), 1, offsetPixels, fichier) != offsetPixels)
    {
        std::cout << "Erreur lecture en-tete.\n";
        fclose(fichier);
        return false;
    }

    // Se placer au debut des pixels
    fseek(fichier, offsetPixels, SEEK_SET);

    image.pixels.clear();

    // Lecture des pixels BGR
    for (int y = 0; y < hauteur; y++)
    {
        for (int x = 0; x < largeur; x++)
        {
            unsigned char pixel[3];

            if (fread(pixel, 1, 3, fichier) != 3)
            {
                std::cout << "Erreur lecture pixel.\n";
                fclose(fichier);
                return false;
            }

            image.pixels.push_back(pixel[0]);
            image.pixels.push_back(pixel[1]);
            image.pixels.push_back(pixel[2]);
        }

        // Ignorer le padding
        fseek(fichier, padding, SEEK_CUR);
    }

    fclose(fichier);

    std::cout << "Largeur : " << image.largeur << '\n';
    std::cout << "Hauteur : " << image.hauteur << '\n';
    std::cout << "Padding : " << padding << '\n';
    std::cout << "Octets stockes : " << image.pixels.size() << '\n';

    return true;
}

bool sauvegarderImage(const char *nomFichier, const BmpImage &image)
{
    FILE *destination = nullptr;
    fopen_s(&destination, nomFichier, "wb");

    if (destination == nullptr)
    {
        std::cout << "Erreur ouverture destination.\n";
        return false;
    }

    // Ecriture de l'en-tete BMP
    if (fwrite(image.entete.data(), 1, image.entete.size(), destination) !=
        image.entete.size())
    {
        std::cout << "Erreur ecriture en-tete.\n";
        fclose(destination);
        return false;
    }

    int octetsLigne = image.largeur * 3;
    int padding = (4 - (octetsLigne % 4)) % 4;
    int index = 0;

    // Ecriture des pixels
    for (int y = 0; y < image.hauteur; y++)
    {
        for (int x = 0; x < image.largeur; x++)
        {
            if (fwrite(&image.pixels[index], 1, 3, destination) != 3)
            {
                std::cout << "Erreur ecriture pixel.\n";
                fclose(destination);
                return false;
            }

            index += 3;
        }

        // Ecriture du padding
        unsigned char zero = 0;

        for (int p = 0; p < padding; p++)
        {
            if (fwrite(&zero, 1, 1, destination) != 1)
            {
                std::cout << "Erreur ecriture padding.\n";
                fclose(destination);
                return false;
            }
        }
    }

    if (fclose(destination) != 0)
    {
        std::cout << "Erreur fermeture destination.\n";
        return false;
    }

    std::cout << "Image sauvegardee : " << nomFichier << '\n';

    return true;
}
