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
