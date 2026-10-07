# Encodeur JPEG en C

Encodeur JPEG *baseline* écrit en C99, sans bibliothèque externe : il convertit des images **PGM** (niveaux de gris) et **PPM** (couleur) en fichiers **JPEG** conformes à la norme [ITU-T T.81](https://www.w3.org/Graphics/JPEG/itu-t81.pdf), avec sous-échantillonnage des chrominances (4:4:4, 4:2:2, 4:2:0 et variantes).

> *English summary: a from-scratch baseline JPEG encoder in C99 (PGM/PPM → JPEG): RGB→YCbCr, chroma subsampling, 8×8 DCT, zigzag, quantization, DC/AC coding and Huffman coding. Team project of 3 students, ENSIMAG (Grenoble INP), 2024.*

Projet réalisé en équipe de 3 à l'ENSIMAG (Grenoble INP), en 2024.

## Utilisation

```bash
make                                               # compile l'exécutable ppm2jpeg
./ppm2jpeg images/color.ppm                        # écrit images/color.jpg
./ppm2jpeg images/color.ppm --outfile=out/c.jpg --sample=2x2,1x1,1x1   # 4:2:0
```

| Option | Rôle |
|---|---|
| `--outfile=FICHIER` | nom du fichier JPEG produit (par défaut : l'entrée avec l'extension `.jpg`) |
| `--sample=HxV,HxV,HxV` | facteurs d'échantillonnage de Y, Cb et Cr (images couleur) |
| `--help` | affiche l'aide |

Exemples de `--sample` : `1x1,1x1,1x1` (4:4:4, par défaut), `2x1,1x1,1x1` (4:2:2), `2x2,1x1,1x1` (4:2:0), `2x2,2x1,2x1`. Chaque facteur doit être compris entre 1 et 4, diviser le plus grand facteur de sa direction, et une MCU ne peut pas contenir plus de 10 blocs. L'option est ignorée pour une image en niveaux de gris.

L'encodeur accepte les PGM/PPM binaires (`P5`/`P6`) 8 bits, avec ou sans commentaires dans l'en-tête, de n'importe quelle taille jusqu'à 65535 × 65535 : si les dimensions ne sont pas des multiples de la taille d'une MCU, l'image est complétée en recopiant ses bords, et le fichier JPEG indique bien les dimensions d'origine.

Autres commandes : `make test`, `make debug` (sans optimisation, avec symboles de débogage), `make tables` (voir plus bas), `make clean`.

## Chaîne de compression

```
image PGM/PPM
   │  lecture, complétée par recopie des bords                       (lecture_fichier_entree)
   ▼
découpage en blocs 8×8                                               (MCU)
   │
   ▼
RGB → YCbCr, sous-échantillonnage des chrominances, ordre des MCU    (converterYCbCr, echantillonnage)
   │
   ▼  pour chaque bloc 8×8 :
DCT  →  zigzag  →  quantification                                    (dct, zigzag, quantification)
   │
   ▼
DC en différentiel, AC en RLE, codes de Huffman, flux de bits        (codage_ACDC_couleur, tableaux_huffman)
   │
   ▼
en-têtes (APP0, DQT, SOF0, DHT, SOS) + données + EOI                 (ecriture_entete_JPEG, ppm2jpeg)
```

Quelques choix techniques :
- **DCT séparable :** la DCT 2D est calculée en deux passes de DCT 1D (lignes puis colonnes), soit 1024 multiplications par bloc au lieu de 4096 avec la formule directe.
- **Sous-échantillonnage générique :** chaque pixel d'une chrominance réduite est la moyenne arrondie du rectangle de pixels qu'il remplace ; les blocs sont écrits dans l'ordre de la norme (Y, puis Cb, puis Cr, pour chaque MCU).
- **Quantification :** la division tronque vers zéro. Sur les images de test, l'écart moyen avec l'original reste de l'ordre de 1 sur 255.
- **Codes de Huffman :** ceux des tables standard de la norme, calculés par `tools/genere_huffman.c`.

## Organisation du code

| Dossier | Contenu |
|---|---|
| `src/` | code source de l'encodeur (`ppm2jpeg.c` est le point d'entrée) |
| `include/` | tables standard de quantification (tables de GIMP) et de Huffman |
| `tools/` | `genere_huffman.c`, qui produit `src/tableaux_huffman.c` (`make tables`) |
| `images/` | petites images de test, générées par `tests/gen_images.py` |
| `tests/` | tests automatiques |

## Tests

```bash
make test
```

`tests/check_jpeg.py` encode les images de `images/` dans 13 configurations (gris, couleur, dimensions impaires, en-tête avec commentaires, 4:4:4, 4:2:2, 4:2:0 et variantes) et, avec [Pillow](https://pillow.readthedocs.io) et NumPy (`pip install pillow numpy`), vérifie que chaque JPEG se décode, qu'il a les bonnes dimensions et le bon échantillonnage, et que l'écart moyen avec l'original reste faible. `tests/run_tests.sh` vérifie ensuite que les arguments ou fichiers invalides sont refusés avec un code d'erreur. Les mêmes tests tournent sur GitHub Actions à chaque push.

## Auteurs

Erwan Didelot, Alexandre Raynaud, Sarangan Sivalingan.
