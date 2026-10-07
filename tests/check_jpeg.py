#!/usr/bin/env python3
"""Encode les images de test dans plusieurs configurations et vérifie les JPEG obtenus.

Pour chaque cas : le fichier commence par SOI et finit par EOI, Pillow le décode, il a
les dimensions de l'image d'origine, l'échantillonnage lu par Pillow est le bon, et
l'écart moyen avec l'original reste faible.  Nécessite Pillow et NumPy.
"""
import subprocess
import sys

try:
    import numpy as np
    from PIL import Image, JpegImagePlugin
except ImportError:
    print("Pillow/NumPy absents : vérification de décodage ignorée (pip install pillow numpy)")
    sys.exit(0)

# Valeurs de JpegImagePlugin.get_sampling : 0 = 4:4:4, 1 = 4:2:2, 2 = 4:2:0, -1 = autre.
# (image, options, échantillonnage attendu ou None, écart moyen maximal sur 255)
CAS = [
    ("gradient.pgm",   [],                      None, 3.0),
    ("whitespace.pgm", [],                      None, 3.0),
    ("color.ppm",      [],                      0,    3.0),
    ("color.ppm",      ["--sample=1x1,1x1,1x1"], 0,   3.0),
    ("color.ppm",      ["--sample=2x1,1x1,1x1"], 1,   3.0),
    ("color.ppm",      ["--sample=2x2,1x1,1x1"], 2,   3.0),
    ("color.ppm",      ["--sample=1x2,1x1,1x1"], -1,  3.0),
    ("color.ppm",      ["--sample=2x2,2x1,2x1"], -1,  3.0),
    ("odd-size.ppm",   [],                      0,    3.0),
    ("odd-size.ppm",   ["--sample=2x1,1x1,1x1"], 1,   3.0),
    ("odd-size.ppm",   ["--sample=2x2,1x1,1x1"], 2,   3.0),
    ("comment.ppm",    [],                      0,    3.0),
    ("comment.ppm",    ["--sample=2x2,1x1,1x1"], 2,   3.0),
]

echecs = 0
for numero, (image, options, sampling_attendu, ecart_max) in enumerate(CAS):
    sortie = f"out/test{numero}.jpg"
    commande = ["./ppm2jpeg", f"images/{image}", f"--outfile={sortie}", *options]
    libelle = f"{image} {' '.join(options)}".strip()
    resultat = subprocess.run(commande, capture_output=True, text=True)
    if resultat.returncode != 0:
        print(f"[ÉCHEC] {libelle} : l'encodeur a échoué ({resultat.stderr.strip()})")
        echecs += 1
        continue

    donnees = open(sortie, "rb").read()
    if donnees[:2] != b"\xff\xd8" or donnees[-2:] != b"\xff\xd9":
        print(f"[ÉCHEC] {libelle} : marqueurs SOI/EOI absents")
        echecs += 1
        continue

    original = Image.open(f"images/{image}")
    decode = Image.open(sortie)
    decode.load()
    problemes = []
    if decode.size != original.size:
        problemes.append(f"dimensions {decode.size} au lieu de {original.size}")
    if sampling_attendu is not None and JpegImagePlugin.get_sampling(decode) != sampling_attendu:
        problemes.append(f"échantillonnage {JpegImagePlugin.get_sampling(decode)} au lieu de {sampling_attendu}")
    ecart = None
    if decode.size == original.size:
        a = np.asarray(original, dtype=int)
        b = np.asarray(decode.convert(original.mode), dtype=int)
        ecart = float(np.abs(a - b).mean())
        if ecart > ecart_max:
            problemes.append(f"écart moyen {ecart:.2f} > {ecart_max}")

    if problemes:
        print(f"[ÉCHEC] {libelle} : " + " ; ".join(problemes))
        echecs += 1
    else:
        print(f"[OK   ] {libelle} : écart moyen {ecart:.2f}")

print(f"\n{len(CAS) - echecs}/{len(CAS)} cas réussis")
sys.exit(1 if echecs else 0)
