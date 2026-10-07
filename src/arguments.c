#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arguments.h"

void afficher_aide(const char *nom_programme) {
    printf("Utilisation : %s <image.pgm|image.ppm> [options]\n\n", nom_programme);
    printf("Encode une image PGM (niveaux de gris) ou PPM (couleur) en JPEG.\n\n");
    printf("Options :\n");
    printf("  --outfile=FICHIER   nom du fichier JPEG produit (par défaut : l'entrée avec l'extension .jpg)\n");
    printf("  --sample=HxV,HxV,HxV  facteurs d'échantillonnage de Y, Cb et Cr (images couleur uniquement)\n");
    printf("                      exemples : 1x1,1x1,1x1 (défaut)   2x1,1x1,1x1   2x2,1x1,1x1\n");
    printf("  --help              affiche cette aide\n");
}

/* Construit le nom de sortie par défaut : l'extension de l'entrée est remplacée par .jpg. */
static char *nom_sortie_par_defaut(const char *entree) {
    /* On ne cherche le point que dans le dernier élément du chemin. */
    const char *debut_nom = entree;
    for (const char *p = entree; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\') debut_nom = p;
    }
    const char *point = NULL;
    for (const char *p = debut_nom; *p != '\0'; p++) {
        if (*p == '.') point = p;
    }
    const size_t base = (point != NULL) ? (size_t)(point - entree) : strlen(entree);

    char *sortie = malloc(base + strlen(".jpg") + 1);
    if (sortie == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    memcpy(sortie, entree, base);
    strcpy(sortie + base, ".jpg");
    return sortie;
}

/* Analyse la valeur de --sample (par exemple "2x2,1x1,1x1") ; quitte si elle est invalide. */
static Echantillonnage analyser_echantillonnage(const char *valeur) {
    int h[3], v[3], lus = 0;
    if (sscanf(valeur, "%dx%d,%dx%d,%dx%d%n", &h[0], &v[0], &h[1], &v[1], &h[2], &v[2], &lus) != 6
        || valeur[lus] != '\0') {
        fprintf(stderr, "Erreur : format de --sample invalide (« %s »). Attendu : HxV,HxV,HxV, par exemple 2x2,1x1,1x1\n", valeur);
        exit(EXIT_FAILURE);
    }

    Echantillonnage e;
    for (int c = 0; c < 3; c++) {
        /* Valeurs hors de [0, 255] ramenées à 0 pour que la vérification les rejette. */
        e.h[c] = (h[c] >= 0 && h[c] <= 255) ? (uint8_t)h[c] : 0;
        e.v[c] = (v[c] >= 0 && v[c] <= 255) ? (uint8_t)v[c] : 0;
    }
    const char *erreur = echantillonnage_verifier(&e);
    if (erreur != NULL) {
        fprintf(stderr, "Erreur : --sample=%s : %s\n", valeur, erreur);
        exit(EXIT_FAILURE);
    }
    return e;
}

void analyser_arguments(int argc, char *argv[], Options *options) {
    options->fichier_entree = NULL;
    options->fichier_sortie = NULL;
    options->echantillonnage = echantillonnage_par_defaut();
    options->echantillonnage_demande = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            afficher_aide(argv[0]);
            exit(EXIT_SUCCESS);
        } else if (strncmp(argv[i], "--outfile=", 10) == 0) {
            free(options->fichier_sortie); /* si l'option est répétée, la dernière l'emporte */
            options->fichier_sortie = malloc(strlen(argv[i] + 10) + 1);
            if (options->fichier_sortie == NULL) {
                perror("Erreur d'allocation mémoire");
                exit(EXIT_FAILURE);
            }
            strcpy(options->fichier_sortie, argv[i] + 10);
        } else if (strncmp(argv[i], "--sample=", 9) == 0) {
            options->echantillonnage = analyser_echantillonnage(argv[i] + 9);
            options->echantillonnage_demande = 1;
        } else if (strncmp(argv[i], "--", 2) == 0) {
            fprintf(stderr, "Erreur : option inconnue « %s » (voir --help)\n", argv[i]);
            exit(EXIT_FAILURE);
        } else if (options->fichier_entree == NULL) {
            options->fichier_entree = argv[i];
        } else {
            fprintf(stderr, "Erreur : un seul fichier d'entrée est accepté (« %s » en trop)\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    if (options->fichier_entree == NULL) {
        fprintf(stderr, "Erreur : aucune image à encoder\n\n");
        afficher_aide(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (options->fichier_sortie == NULL) {
        options->fichier_sortie = nom_sortie_par_defaut(options->fichier_entree);
    }
}

void liberer_options(Options *options) {
    free(options->fichier_sortie);
    options->fichier_sortie = NULL;
}
