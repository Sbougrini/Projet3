#include "BmpImage.h"

#include <iostream>

#include <cstdio>

#include <filesystem>

#include <cstdint>

int ouvrirFichier(const char* nomFichier)
{
	FILE* fichier = nullptr;

	std::cout << "Fichier existe : "
		<< std::filesystem::exists(nomFichier)
		<< std::endl;

	fopen_s(&fichier,nomFichier, "rb");
	
	if (fichier == nullptr)
	{
		std::cout << "Erreur ouverture fichier." << std::endl;
		return 1;
	}

	std::cout << "Fichier ouvert " << std::endl;

	char signature[2];
	size_t octetsLus = fread(signature, sizeof(char), 2, fichier);

	if (octetsLus != 2)
	{
		std::cout << "Erreur lecture signature." << std::endl;
		fclose(fichier);
		return 1;
	}

	if (signature[0] != 'B' || signature[1] != 'M')
	{
		std::cout << "Format BMP invalide." << std::endl;
		fclose(fichier);
		return 1;
	}

	std::cout << "Signature : " << signature[0] << signature[1] << std::endl;

	int32_t largeur = 0;

	fseek(fichier, 18, SEEK_SET);
	size_t octetsLusLargeur = fread(&largeur, sizeof(int32_t), 1, fichier);

	if (octetsLusLargeur != 1)
	{
		std::cout << "Erreur lecture largeur." << std::endl;
		fclose(fichier);
		return 1;
	}

	std::cout << "Largeur : " << largeur << std::endl;

	int32_t hauteur = 0;

	fseek(fichier, 22, SEEK_SET);
	size_t octetsLusHauteur = fread(&hauteur, sizeof(int32_t), 1, fichier);

	if (octetsLusHauteur != 1)
	{
		std::cout << "Erreur lecture hauteur." << std::endl;
		fclose(fichier);
		return 1;
	}

	std::cout << "Hauteur : " << hauteur << std::endl;

	fclose(fichier);
	return 0;
}

