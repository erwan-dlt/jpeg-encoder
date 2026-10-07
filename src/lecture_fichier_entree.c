#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lecture_fichier_entree.h"

/* Taille en pixels d'un bloc de codage. */
#define TAILLE_BLOC 8
/* Les dimensions de l'image sont stockées sur 16 bits dans l'en-tête JPEG. */
#define DIMENSION_MAX 65535

void freeDonneesRGB(FileData *donnees_RGB) {
    for (int i = 0; i < donnees_RGB->hauteur; i++) {
        free(donnees_RGB->donnees[i]);
    }
    free(donnees_RGB->donnees);
}

/* Affiche un message d'erreur, ferme le fichier et quitte. */
static void erreur_lecture(FILE *file, const char *message) {
    fprintf(stderr, "Erreur : %s\n", message);
    if (file != NULL) fclose(file);
    exit(EXIT_FAILURE);
}

/* Lit un octet de pixel ; quitte avec une erreur si le fichier est tronqué. */
static void lire_octet(FILE *file, uint8_t *octet) {
    if (fread(octet, sizeof(uint8_t), 1, file) != 1) {
        erreur_lecture(file, "fichier image tronqué");
    }
}

/*
 * Lit un entier de l'en-tête PNM en sautant les espaces et les commentaires
 * ('#' jusqu'à la fin de la ligne). Consomme exactement un caractère après le
 * nombre : pour la dernière valeur de l'en-tête (l'intensité maximale), c'est
 * l'unique espace qui précède les données, ce qui évite d'avaler par erreur un
 * premier pixel dont la valeur ressemble à un espace.
 * Retourne -1 si aucun entier valide n'est trouvé.
 */
static int lire_entier_pnm(FILE *file) {
    int c = fgetc(file);
    while (c == '#' || isspace(c)) {
        if (c == '#') {
            while (c != '\n' && c != EOF) c = fgetc(file);
        } else {
            c = fgetc(file);
        }
    }
    if (!isdigit(c)) return -1;

    int valeur = 0;
    while (isdigit(c)) {
        valeur = valeur * 10 + (c - '0');
        if (valeur > 10 * DIMENSION_MAX) return -1;
        c = fgetc(file);
    }
    if (c == '#') ungetc(c, file); /* un commentaire suit : il sera sauté à la lecture suivante */
    return valeur;
}

/* Arrondit n au multiple de pas immédiatement supérieur (ou égal). */
static int arrondi_au_multiple(int n, int pas) {
    return ((n + pas - 1) / pas) * pas;
}

FileData spec_image(const char *nom_fichier, int facteur_h, int facteur_v) {
    FileData image;
    FILE *file = fopen(nom_fichier, "rb"); /* mode binaire : indispensable sous Windows */
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    /* Nombre magique : P5 (PGM binaire) ou P6 (PPM binaire). */
    image.type_file[0] = (char)fgetc(file);
    image.type_file[1] = (char)fgetc(file);
    image.type_file[2] = '\0';
    if (strcmp(image.type_file, "P5") != 0 && strcmp(image.type_file, "P6") != 0) {
        erreur_lecture(file, "type d'image non géré (seuls les formats binaires P5 et P6 le sont)");
    }

    /* Largeur, hauteur, intensité maximale. */
    image.largeur = lire_entier_pnm(file);
    image.hauteur = lire_entier_pnm(file);
    image.intensite = lire_entier_pnm(file);
    if (image.largeur < 1 || image.largeur > DIMENSION_MAX ||
        image.hauteur < 1 || image.hauteur > DIMENSION_MAX) {
        erreur_lecture(file, "dimensions de l'image invalides (de 1 à 65535 pixels attendus)");
    }
    if (image.intensite != 255) {
        erreur_lecture(file, "seules les images 8 bits (intensité maximale 255) sont gérées");
    }
    image.largeur_originale = image.largeur;
    image.hauteur_originale = image.hauteur;

    /* Une image en niveaux de gris n'est jamais sous-échantillonnée. */
    const int est_couleur = (strcmp(image.type_file, "P6") == 0);
    if (!est_couleur) {
        facteur_h = 1;
        facteur_v = 1;
    }

    /* Dimensions complétées pour former un nombre entier de MCU. */
    const int nouvelle_largeur = arrondi_au_multiple(image.largeur, TAILLE_BLOC * facteur_h);
    const int nouvelle_hauteur = arrondi_au_multiple(image.hauteur, TAILLE_BLOC * facteur_v);

    /* Allocation des lignes de pixels. */
    image.donnees = malloc(nouvelle_hauteur * sizeof(RGB *));
    if (image.donnees == NULL) {
        erreur_lecture(file, "allocation mémoire impossible pour les lignes de l'image");
    }
    for (int i = 0; i < nouvelle_hauteur; i++) {
        image.donnees[i] = malloc(nouvelle_largeur * sizeof(RGB));
        if (image.donnees[i] == NULL) {
            erreur_lecture(file, "allocation mémoire impossible pour les colonnes de l'image");
        }
    }

    /* Lecture des pixels. */
    for (int i = 0; i < image.hauteur; i++) {
        for (int j = 0; j < image.largeur; j++) {
            RGB *pixel = &image.donnees[i][j];
            if (est_couleur) {
                lire_octet(file, &pixel->R);
                lire_octet(file, &pixel->G);
                lire_octet(file, &pixel->B);
            } else {
                uint8_t gris;
                lire_octet(file, &gris);
                pixel->R = pixel->G = pixel->B = gris;
            }
        }
    }
    fclose(file);

    /* Complète l'image : on recopie la dernière colonne, puis la dernière ligne
     * (ce qui recopie aussi le coin). Le décodeur ignore ces pixels en trop. */
    for (int i = 0; i < image.hauteur; i++) {
        for (int j = image.largeur; j < nouvelle_largeur; j++) {
            image.donnees[i][j] = image.donnees[i][image.largeur - 1];
        }
    }
    for (int i = image.hauteur; i < nouvelle_hauteur; i++) {
        for (int j = 0; j < nouvelle_largeur; j++) {
            image.donnees[i][j] = image.donnees[image.hauteur - 1][j];
        }
    }
    image.largeur = nouvelle_largeur;
    image.hauteur = nouvelle_hauteur;
    return image;
}
