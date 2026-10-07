/**
 * @file arguments.h
 * @brief Analyse de la ligne de commande de ppm2jpeg.
 */
#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include "echantillonnage.h"

/** Options déduites de la ligne de commande. */
typedef struct {
    const char *fichier_entree;   /* image PGM/PPM à encoder (pointe dans argv) */
    char *fichier_sortie;         /* fichier JPEG à écrire (alloué : voir liberer_options) */
    Echantillonnage echantillonnage; /* facteurs --sample, 4:4:4 par défaut */
    int echantillonnage_demande;  /* 1 si l'option --sample a été donnée */
} Options;

/** Affiche l'aide sur la sortie standard. */
void afficher_aide(const char *nom_programme);

/**
 * @brief Analyse les arguments de la ligne de commande.
 *
 * Quitte le programme (code 0 pour --help, code d'erreur sinon) si les arguments
 * sont invalides ou incomplets.
 */
void analyser_arguments(int argc, char *argv[], Options *options);

/** Libère la mémoire allouée par analyser_arguments. */
void liberer_options(Options *options);

#endif /* ARGUMENTS_H */
