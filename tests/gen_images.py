#!/usr/bin/env python3
"""Génère les images de test (PGM/PPM) du dossier images/. Aucune dépendance."""
import os

OUT = os.path.join(os.path.dirname(__file__), "..", "images")


def ecrire(nom, entete, donnees):
    with open(os.path.join(OUT, nom), "wb") as fh:
        fh.write(entete.encode())
        fh.write(bytes(donnees))


def pixels_gris(w, h, f):
    return [f(x, y) for y in range(h) for x in range(w)]


def pixels_rgb(w, h, f):
    return [c for y in range(h) for x in range(w) for c in f(x, y)]


os.makedirs(OUT, exist_ok=True)

# Niveaux de gris 64x64 : dégradé alterné avec un damier de blocs 8x8
ecrire("gradient.pgm", "P5\n64 64\n255\n",
       pixels_gris(64, 64, lambda x, y: int(255 * (x + y) / 126) if (x // 8 + y // 8) % 2 == 0 else int(255 * (1 - x / 63))))

# Couleur 96x64 : dégradés sur les trois canaux, avec des couleurs très saturées dans les coins
ecrire("color.ppm", "P6\n96 64\n255\n",
       pixels_rgb(96, 64, lambda x, y: (int(255 * x / 95), int(255 * y / 63), int(255 * (1 - x / 95)))))

# Couleur 50x37 : dimensions qui ne sont pas des multiples de 8
ecrire("odd-size.ppm", "P6\n50 37\n255\n",
       pixels_rgb(50, 37, lambda x, y: (int(255 * x / 49), 128, int(255 * y / 36))))

# Gris 16x16 dont les premiers pixels valent 32, 10, 9, 13, 11 : ce sont des codes d'espaces,
# qu'un lecteur d'en-tête mal écrit avalerait en croyant lire des séparateurs.
debut = [32, 10, 9, 13, 11]
ecrire("whitespace.pgm", "P5\n16 16\n255\n",
       debut + pixels_gris(16, 16, lambda x, y: 8 * (x + y))[len(debut):])

# Couleur 32x16 avec des commentaires dans l'en-tête, comme en écrivent GIMP ou ImageMagick
ecrire("comment.ppm", "P6\n# image de test avec commentaires\n32 # largeur\n16\n# intensité\n255\n",
       pixels_rgb(32, 16, lambda x, y: (8 * x, 16 * y, 128)))
