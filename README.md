# Projet3 : Steganographie
BOUGRINI Sami
BAURIN Nathan

Dans ce projet nous avions pour but de cacher un message dans une image BMP, nous nous sommes donc partagé le travail à deux : 

## Gestion des images BMP et insertion du message

#### Développeur Nathan BAURIN
## 1. Objectif de ma partie

Ma partie consistais a charger une image BMP, lire ses pixels, cacher un message grâce à la méthode LSB ainsi que de sauvegarder la nouvelle image avec le message caché.

Les fichiers concernés sont :

`BmpImage.h` et `BmpImage.cpp` : chargement et sauvegarde BMP.

`Steganography.h` et `Steganography.cpp` : insertion LSB et métadonnées.

## 2. Lecture de l'image BMP

La fonction loadImage(const char* fileName, BmpImage& image, std::string& errorMessage) ouvre le fichier en mode binaire (rb) et vérifie :
- la signature BM ;
- des dimensions strictement positives ;
- une profondeur de 24 bits par pixel ;
- l'absence de compression ;
- un en-tête DIB de 40 octets ;
- un décalage vers les pixels d'au moins 54 octets ;
- la présence des données nécessaires à toutes les lignes de pixels.

Les données sont conservées dans la structure `BmpImage`

| Champ | Utilité |
|---|---|
| `width`, `height` | Dimensions de l'image |
| `pixelOffset` | Position des données des pixels dans le fichier |
| `header` | Octets précédant les pixels |
| `pixels` | Composantes de couleur dans l'ordre BGR |
| `paddings` | Octets de remplissage de chaque ligne |


