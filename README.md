# Projet3 : Steganographie
BOUGRINI Sami
BAURIN Nathan

Dans ce projet nous avions pour but de cacher un message dans une image BMP, nous nous sommes donc partagé le travail à deux. Après la compilation et l'utilisation, chacun présente sa partie, et la répartition du travail est détaillée à la fin.

## Compilation

- **Environnement :** Visual Studio 2026 (outils de génération v145), norme **C++20**.
- Ouvrir la solution **`Projet3.slnx`**.
- Choisir la configuration **Debug** ou **Release**, plateforme **x64**.
- Compiler avec **Générer → Générer la solution** (`Ctrl+Maj+B`).
- L'exécutable est créé dans `x64\Debug\Projet3.exe` (ou `x64\Release\Projet3.exe`).

Le projet utilise `fopen` comme demandé dans le sujet. Visual Studio le signale comme « non sécurisé » (avertissement C4996) : nous avons donc ajouté `_CRT_SECURE_NO_WARNINGS` dans **Propriétés → C/C++ → Préprocesseur → Définitions de préprocesseur**, pour Debug et pour Release.

## Utilisation

```
Projet3.exe help
Projet3.exe embed <source.bmp> <destination.bmp> "<message>" [--key "<cle>"]
Projet3.exe extract <source.bmp> [--key "<cle>"]
```

- Les crochets `[ ]` indiquent une option facultative.
- Les chemins et les messages qui contiennent des espaces doivent être écrits **entre guillemets**.
- Le programme renvoie **0** si tout s'est bien passé, **1** en cas d'erreur (le message d'erreur est affiché).

**Lancer le programme :**
- depuis Visual Studio : **Projet → Propriétés → Débogage → Arguments de la commande**, puis `Ctrl+F5` ;
- depuis un terminal ouvert dans le dossier `Projet3` (celui qui contient `images/`) : `..\x64\Debug\Projet3.exe help`.

### Exemples

Cacher un message sans clé :
```
Projet3.exe embed "images/Gaming_Campus_Logo.bmp" "images/logo secret.bmp" "rdv a 17h"
```

Cacher un message chiffré avec une clé :
```
Projet3.exe embed "images/Gaming_Campus_Logo.bmp" "images/logo chiffre.bmp" "rdv a 17h" --key "ma cle"
```

Retrouver les messages :
```
Projet3.exe extract "images/logo secret.bmp"
Projet3.exe extract "images/logo chiffre.bmp" --key "ma cle"
```

Le fichier de destination ne doit pas exister : le programme refuse d'écraser un fichier.

### Format supporté

Images **BMP 24 bits non compressées**, avec un en-tête de 40 octets (`BITMAPINFOHEADER`), comme celles enregistrées par Paint. Les autres variantes (32 bits, compressées, en-tête de 124 octets) sont refusées avec un message d'erreur.

Capacité : `(largeur × hauteur × 3) / 8 − 9` octets de message (9 octets de métadonnées). Par exemple, `images/Image.bmp` (8 × 8) peut contenir 15 octets.

## Organisation du code

| Fichiers | Rôle | Auteur |
|---|---|---|
| `Projet3.cpp` | `main` : appelle seulement `Application::run` | Sami |
| `Application.h/.cpp` | relie tous les modules (embed et extract) | Sami |
| `CommandLine.h/.cpp` | lecture et vérification de la ligne de commande, aide | Sami |
| `Xor.h/.cpp` | chiffrement et déchiffrement XOR | Sami |
| `BmpImage.h/.cpp` | chargement, vérification et sauvegarde du BMP | Nathan |
| `Steganography.h/.cpp` | insertion (Nathan) et extraction (Sami) du message | Nathan et Sami |

Les modules ne s'affichent rien eux-mêmes : chaque fonction qui peut échouer renvoie un `bool` et remplit un message d'erreur, et c'est `Application` qui affiche.

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

Les pixels sont lus avec fread().


#### Gestion du padding

Chaque ligne d'un BMP doit occuper un nombre d'octets multiple de quatre.

Le programme calcule donc le padding :

```cpp
int rowBytes = width * BYTES_PER_PIXEL;
int padding = (BYTE_ALIGNMENT - (rowBytes % BYTE_ALIGNMENT))
              % BYTE_ALIGNMENT;
```

Les octets de padding sont conservés dans `paddings` pour pouvoir les réécrire lors de la sauvegarde.

### 3. Insertion d'un message avec LSB

La fonction `hideMessage()` permet de cacher un message dans les composantes de couleur.

La méthode LSB (*Least Significant Bit*) consiste à modifier uniquement le bit de poids faible d'un octet.

Pour cacher un bit, j'utilise :

```cpp
image.pixels[index] =
    (image.pixels[index] & 0xFE) | bit;
```

Explication :

- `& 0xFE` met le dernier bit à zéro.
- `| bit` écrit le bit du message.
- `++index` passe à la composante suivante.

Chaque caractère est traité comme un octet. Un octet du message nécessite huit composantes de couleur.

Cette méthode permet de cacher des données en modifiant chaque composante utilisée de 1 au maximum.

### 4. Organisation des métadonnées

Avant le message, le programme cache neuf octets de métadonnées.

| Information | Taille | Utilité |
|---|---|---|
| Signature `STEG` | 4 octets | Identifier le format |
| Indicateur de chiffrement | 1 octet | Indiquer si le message est chiffré |
| Longueur du message | 4 octets | Connaître sa taille |
| Message | Variable | Contenu à cacher |

L'indicateur de chiffrement vaut :

- `0` : message non chiffré.
- `1` : message chiffré.

La longueur est enregistrée sur quatre octets en little-endian.

Les bits de chaque octet sont insérés du bit de poids fort vers le bit de poids faible.

La fonction `hideMessage()` ne chiffre pas directement le message. Le chiffrement XOR est géré dans un autre module.

### 5. Vérification de la capacité

Avant l'insertion, le programme vérifie que l'image possède suffisamment de place.

La capacité disponible est calculée avec :

```cpp
size_t capacity =
    image.pixels.size() / BITS_PER_BYTE - METADATA_SIZE;
```

Ce calcul est effectué après avoir vérifié que l'image possède au moins 72 composantes pour les métadonnées.

Si le message est trop grand, la fonction retourne `false` et renseigne `errorMessage`.

La longueur du message est également limitée à la valeur maximale représentable sur 32 bits.

### 6. Sauvegarde de l'image

La fonction `saveImage()` permet de créer un nouveau fichier BMP.

Elle réécrit :

1. L'en-tête original.
2. Les pixels, éventuellement modifiés par le LSB.
3. Les octets de padding conservés.

La sauvegarde utilise `fwrite()` pour écrire les données et `fclose()` pour fermer le fichier.

Le fichier est ouvert avec :

```cpp
FILE* destination = fopen(fileName, "wbx");
```

Le mode `wbx` permet de créer un fichier sans écraser un fichier déjà existant.

### 7. Tests de ma partie

| Test | Résultat |
|---|---|
| Ouverture de `Image.bmp` | Fonctionnelle lors des essais |
| Lecture des pixels BGR | Fonctionnelle lors des essais |
| Insertion du message `Bonjour` | Effectuée pendant le développement |
| Sauvegarde de `Image_cachee.bmp` | Effectuée pendant le développement |
| Calcul du padding | Implémenté |
| Vérification de capacité | Implémentée |
| Refus d'écraser un fichier existant | Implémenté |

Ces essais ont été réalisés pendant le développement. Ils doivent être rejoués après la fusion des branches.
### 8. Limites de ma partie

- Seules les images BMP 24 bits non compressées avec un en-tête DIB de 40 octets sont acceptées.
- Les images avec une hauteur négative ne sont pas prises en charge.
- Le message ne doit pas dépasser la capacité disponible.
- Le LSB permet de dissimuler un message, mais ne le chiffre pas.
- Les éventuelles données supplémentaires situées après les pixels ne sont pas conservées lors de la sauvegarde.

### 9. Sources utilisées

#### Format BMP

- [Microsoft Learn - Bitmap Storage](https://learn.microsoft.com/en-us/windows/win32/gdi/bitmap-storage) : organisation des fichiers BMP.
- [Microsoft Learn - BITMAPINFOHEADER](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader) : informations sur les dimensions, les couleurs et les lignes.
- [Wikipedia - BMP file format](https://en.wikipedia.org/wiki/BMP_file_format) : structure des en-têtes, pixels et padding.

#### Fonctions C et C++

- [Microsoft Learn - fopen](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen?view=msvc-170) : ouverture des fichiers et modes utilisés.
- [cppreference - C-style file I/O](https://en.cppreference.com/w/cpp/io/c) : documentation de `fread()`, `fwrite()`, `fseek()`, `ftell()` et `fclose()`.

#### Stéganographie

- [Wikipedia - Steganography](https://en.wikipedia.org/wiki/Steganography) : principe de la stéganographie.
- [Wikipedia - Bit numbering](https://en.wikipedia.org/wiki/Bit_numbering) : explication du bit de poids faible.


## Ligne de commandes, chiffrement XOR et extraction du message


#### Développeur Sami BOUGRINI
## 1. Objectif de ma partie

Ma partie consistait à vérifier la ligne de commande tapée par l’utilisateur, à chiffrer le message à cacher, à extraire ce message de l’image, puis à relier tous les modules entre eux.

Les fichiers concernés sont :

`CommandLine.h` et `CommandLine.cpp` : lecture et vérification de la ligne de commande (la commande, le nombre d'arguments, l'option --key).

`Xor.h` et `Xor.cpp` : chiffrer et déchiffrer le message.

Ma partie de `Steganography.h` et `Steganography.cpp` : extraction du message, qu'il soit chiffré ou non.

`Application.h` et `Application.cpp` : l’orchestration du programme qui relie la ligne de commande, le BMP, la stéganographie et le XOR.

`Projet3.cpp` : la fonction main, qui appelle seulement `Application::run`.

## 2. Lecture de la ligne de commande

| Commande | Arguments | Rôle |
|---|---|---|
| `help` | aucun | affiche l'aide |
| `embed` | `<source.bmp> <destination.bmp> "<message>" [--key "<cle>"]` | cache un message dans une image |
| `extract` | `<source.bmp> [--key "<cle>"]` | retrouve le message caché |

Les crochets indiquent un argument facultatif, on peut cacher un message sans définir de clé et, par conséquent, extraire le message sans avoir de clé. Les arguments contenant des espaces
doivent être écrits entre guillemets.

La fonction `verifyCommandLine` convertit d'abord les arguments de `main` en un `std::vector<std::string>`. Chaque argument devient ainsi une `std::string`, que l'on peut comparer directement avec `==` (avec un `char *`, `==` compare des adresses), et le vecteur connaît sa propre taille.

Elle regarde ensuite la commande demandée et appelle la fonction qui la vérifie : `verifyHelp`, `verifyEmbed` ou `verifyExtract`. Chacune vérifie le nombre d'arguments avant de les lire. L'option `--key` est vérifiée par la fonction `verifyKey` qui reçoit la position de l'option.

Les erreurs refusées sont :
- une commande inconnue ;
- un nombre d'arguments incorrect ;
- une option inconnue à la place de `--key` ;
- `--key` sans clé, ou une clé vide ;
- un message à cacher vide.

Les nombres d'arguments sont des constantes, plutôt que des valeurs écrites directement dans le code. Le nombre d'arguments avec une clé se calcule en ajoutant les deux arguments de l'option et la position de `--key` est juste après les arguments obligatoires.

Le résultat est rangé dans la structure `Command` :

| Champ | Utilité |
|---|---|
| `mode` | la commande demandée (`Mode::HELP`, `Mode::EMBED` ou `Mode::EXTRACT`) |
| `source` | le chemin de l'image à lire |
| `destination` | le chemin de l'image à créer (pour `embed`) |
| `message` | le message à cacher (pour `embed`) |
| `key` | la clé de chiffrement (vide s'il n'y en a pas) |

Le résultat est rangé dans un `enum class`. Une faute de frappe dans le nom d'une commande provoque une erreur.
Les fonctions renvoient un `bool` et remplissent un message d'erreur

## 3. Chiffrement XOR

La classe statique `Xor` ne contient qu'une seule méthode, `encryptDecrypt`, déclarée dans `Xor.h` et définie dans `Xor.cpp`.

Elle reçoit un texte et la clé saisie par l'utilisateur, et renvoie le texte transformé. Comme deux XOR avec la même clé s'annulent, la même fonction sert à chiffrer (à l'insertion) et à déchiffrer (à l'extraction). Avec une autre clé, elle renvoie des octets sans signification, sans pouvoir détecter l'erreur.

Un `assert` vérifie d'abord que la clé n'est pas vide. Une clé vide est déjà refusée par la ligne de commande : si elle arrive jusqu'ici, c'est une erreur de programmation. Sans cette vérification, le calcul `i % taille de la clé` provoquerait une division par zéro.

Le texte est ensuite copié dans une variable locale `result`, qui est modifiée puis renvoyée (le texte reçu est constant et ne peut pas être modifié).

Une boucle parcourt chaque octet du message. L'octet n°`i` est combiné par un XOR avec l'octet n°`i % taille de la clé` : le `%` fait boucler l'indice, ce qui répète la clé sur toute la longueur du message.

```
message  :  S  a  l  u  t
indice   :  0  1  2  3  4
clé "ab" :  a  b  a  b  a
```

Le résultat du XOR est converti avec `static_cast<char>` : le XOR de deux `char` donne un `int` (promotion entière), qu'il faut ramener à un `char` pour le ranger dans la chaîne.

Ce chiffrement est volontairement simple : la clé étant répétée, il ne protège pas réellement le message, et une mauvaise clé n'est pas détectée (voir la partie « Limites »).

## 4. Extraction du message

L'extraction se fait avec la fonction `extractMessage` (dans `Steganography.cpp`). Elle reçoit les composantes de l'image (les octets B, G, R sans le padding) et remplit le message, un booléen qui indique s'il est chiffré, et un message d'erreur.

J'ai choisi de lui passer seulement les composantes et pas toute l'image : comme ça, j'ai pu la tester avant que la partie BMP soit finie.

Elle relit les octets dans le même ordre que l'insertion :

| Octets | Contenu |
|---|---|
| 0 à 3 | la signature `STEG` |
| 4 | le drapeau de chiffrement (0 ou 1) |
| 5 à 8 | la longueur du message (little-endian) |
| 9 et + | le message |

Pour lire un octet caché, la fonction `readHiddenByte` prend 8 composantes à la suite. Pour chacune, `& 1` récupère le bit de poids faible, puis `bit << (7 - i)` le remet à sa place dans l'octet. Le premier bit lu est le bit de poids fort, comme à l'insertion.

Avant de lire le message, je vérifie :
- que l'image est assez grande pour contenir les métadonnées ;
- que la signature est bien `STEG` (sinon, il n'y a pas de message) ;
- que le drapeau vaut 0 ou 1 ;
- que la longueur ne dépasse pas la capacité de l'image, pour ne pas lire en dehors.

La longueur est reconstruite à partir de 4 octets, en décalant chacun de 0, 8, 16 ou 24 bits. Je convertis chaque octet en `std::uint32_t` avant de le décaler.

Ensuite, les octets du message sont ajoutés un par un, sans conversion, donc les accents sont conservés. Le message est renvoyé tel quel : c'est `Application` qui le déchiffre s'il y a une clé.

## 5. Orchestration : la classe `Application`

La classe `Application` relie tous les modules entre eux. Le `main` ne fait qu'appeler `Application::run`, comme dans mon projet précédent.

`run` vérifie d'abord la ligne de commande avec `CommandLine`. En cas d'erreur, elle affiche le message et l'aide. Sinon, elle appelle la bonne fonction selon la commande :

- `runEmbed` : charge l'image, chiffre le message avec `Xor` s'il y a une clé, cache le message avec `hideMessage`, puis sauvegarde la nouvelle image ;
- `runExtract` : charge l'image, extrait le message, vérifie qu'une clé est donnée si le message est chiffré, le déchiffre, puis l'affiche.

Si une clé est donnée alors que le message n'est pas chiffré, un avertissement s'affiche et le message est quand même affiché.

Chaque étape vérifie si elle a réussi : en cas d'erreur, le message d'erreur est affiché et le programme s'arrête. Le programme renvoie 0 si tout s'est bien passé, et 1 en cas d'erreur.

`Application` est une classe statique (son constructeur est supprimé), car elle n'a pas de données à garder entre deux appels.

## 6. Tests de ma partie

Les tests ont été faits depuis Visual Studio (arguments de débogage) et depuis un terminal, après la fusion des branches dans `dev`.

#### Ligne de commande

| Test | Commande | Résultat |
|---|---|---|
| Aide | `help` | L'aide s'affiche |
| Aucune commande | *(rien)* | Erreur + aide |
| Commande inconnue | `cacher ...` | Erreur + aide |
| Arguments manquants | `embed image.bmp` | Erreur + aide |
| Trop d'arguments | `extract a.bmp b.bmp` | Erreur + aide |
| Message vide | `embed a.bmp b.bmp ""` | Erreur « message vide » |
| Option inconnue | `embed a.bmp b.bmp "Salut" --cle "x"` | Erreur |
| `--key` sans clé | `extract b.bmp --key` | Erreur |
| Clé vide | `extract b.bmp --key ""` | Erreur |
| Espaces dans les chemins et le message | `embed "images/Gaming_Campus_Logo.bmp" "images/logo secret.bmp" "rdv a 17h"` | Accepté |

#### Chiffrement XOR

| Test | Résultat |
|---|---|
| Chiffrer puis déchiffrer avec la même clé | Le texte d'origine est retrouvé |
| Clé plus courte que le message | La clé est répétée, le texte est retrouvé |
| Déchiffrer avec une autre clé | Des caractères sans signification, sans plantage |

#### Extraction et programme complet

| Test | Résultat |
|---|---|
| Cacher puis extraire sans clé | Message retrouvé |
| Cacher puis extraire avec la même clé | Message retrouvé |
| Extraire un message chiffré sans `--key` | Erreur « ce message est chiffre, utilisez --key » |
| Extraire avec une mauvaise clé | Texte illisible, sans plantage |
| Donner une clé alors que le message n'est pas chiffré | Avertissement, puis le message s'affiche |
| Extraire d'une image sans message | Erreur « Aucun message cache dans l'image » |
| Message à la capacité maximale (`Image.bmp` 8 × 8 : 15 octets) | Accepté et retrouvé |
| Un octet de trop (16 octets) | Refusé |
| Accents dans le message | Retrouvés à l'identique |
| Image avec padding (`Gaming_Campus_Logo.bmp`, 554 × 554) | Message retrouvé, padding intact |
| Destination déjà existante | Refusé, le fichier n'est pas écrasé |
| Fichier absent | Erreur, sans plantage |

J'ai aussi comparé l'image avant et après une insertion dans l'éditeur binaire de Visual Studio : seuls les bits de poids faible des composantes changent, l'en-tête et le padding restent identiques.

## 7. Limites de ma partie

- Le XOR avec une clé répétée ne protège pas vraiment le message : il suffit de connaître une partie du texte pour retrouver la clé.
- Une mauvaise clé n'est pas détectée : le programme affiche des caractères sans signification au lieu d'une erreur.
- Le message est caché et relu octet par octet, sans conversion : les accents s'affichent correctement seulement si la console utilise le même encodage à l'insertion et à l'extraction.
- La clé et le message sont tapés dans la ligne de commande, ils peuvent donc rester visibles dans l'historique du terminal.
- La stéganographie cache le message mais ne le protège pas : quelqu'un qui connaît le format (`STEG`, drapeau, longueur) peut le lire.

## 8. Sources utilisées

#### Ligne de commande et C++

- [cppreference - Main function](https://en.cppreference.com/w/cpp/language/main_function) : `argc`, `argv` et leur conversion.
- [cppreference - std::vector](https://en.cppreference.com/w/cpp/container/vector) et [std::string](https://en.cppreference.com/w/cpp/string/basic_string) : le vecteur d'arguments.
- [cppreference - enum class](https://en.cppreference.com/w/cpp/language/enum) : la structure `Command` et le `Mode`.
- [cppreference - Fixed width integer types](https://en.cppreference.com/w/cpp/types/integer) : `std::uint32_t` pour la longueur du message.

#### Opérations sur les bits et XOR

- [cppreference - Bitwise operators](https://en.cppreference.com/w/cpp/language/operator_arithmetic#Bitwise_logic_operators) : `&`, `|`, `^`, `<<`, utilisés pour lire les bits et pour le XOR.
- [Wikipedia - XOR cipher](https://en.wikipedia.org/wiki/XOR_cipher) : le principe du chiffrement XOR et ses limites.
- Le cours sur les opérations binaires (masques, décalages) vu en classe.

## Répartition du travail

| | Nathan BAURIN | Sami BOUGRINI |
|---|---|---|
| Module principal | `BmpImage` : lecture, vérification, padding, sauvegarde | `CommandLine` : commandes, `--key`, aide, erreurs |
| Stéganographie | insertion du message (LSB), métadonnées, capacité | extraction du message, vérification de la longueur |
| Autres | | `Xor`, `Application` et `main` |
| README | sa partie | sa partie, compilation et utilisation |

Chacun a travaillé sur sa propre branche Git (`Nathan`, `Sami`, `CommandLine`, `XOR`, `Extract`…), fusionnée dans `dev` par Pull Request. Nous avons ensuite testé les deux parties ensemble après la fusion (voir les sections « Tests »).

## Limites communes

- Seules les images BMP 24 bits non compressées sont acceptées.
- La stéganographie cache le message mais ne le protège pas, et le XOR avec une clé répétée est facile à casser.
- Une mauvaise clé n'est pas détectée.
- Le message modifie l'image : enregistrer l'image dans un autre format (JPEG…) ou la retoucher efface le message.
