#!/bin/sh
# Teste l'encodeur : vérification des JPEG produits, puis gestion des erreurs.
set -u
cd "$(dirname "$0")/.."

[ -x ./ppm2jpeg ] || make || exit 1
mkdir -p out
statut=0

# 1. JPEG produits (décodage avec Pillow)
python3 tests/check_jpeg.py || statut=1

# 2. Erreurs de ligne de commande : l'encodeur doit refuser, avec un code de sortie non nul
echo
doit_echouer() {
    libelle=$1; shift
    if ./ppm2jpeg "$@" >/dev/null 2>&1; then
        echo "[ÉCHEC] $libelle : aurait dû échouer"; statut=1
    else
        echo "[OK   ] $libelle : refusé"
    fi
}
doit_echouer "aucun argument"
doit_echouer "image absente"                images/absente.ppm
doit_echouer "option inconnue"              images/color.ppm --rapide
doit_echouer "--sample mal formé"           images/color.ppm --sample=abc
doit_echouer "--sample trop de blocs"       images/color.ppm --sample=3x3,1x1,1x1
doit_echouer "--sample facteur qui ne divise pas" images/color.ppm --sample=2x1,3x1,1x1

# 3. Fichiers invalides
printf 'P6\n10 10\n255\n' > out/tronque.ppm            # aucune donnée de pixel
printf 'P3\n1 1\n255\n0 0 0\n' > out/ascii.ppm         # format texte non géré
printf 'P5\n2 2\n65535\n' > out/seize-bits.pgm         # 16 bits non géré
doit_echouer "image tronquée"               out/tronque.ppm   --outfile=out/x.jpg
doit_echouer "format ASCII (P3)"            out/ascii.ppm     --outfile=out/x.jpg
doit_echouer "image 16 bits"                out/seize-bits.pgm --outfile=out/x.jpg

# 4. --help réussit
if ./ppm2jpeg --help >/dev/null 2>&1; then echo "[OK   ] --help"; else echo "[ÉCHEC] --help"; statut=1; fi

exit $statut
