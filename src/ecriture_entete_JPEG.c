#include <stdio.h>
#include "ecriture_entete_JPEG.h"
#include "../include/qtables.h"
#include "../include/htables.h"

/* Écrit un octet. */
static void ecrire_u8(FILE *file, uint8_t valeur) {
    fwrite(&valeur, 1, 1, file);
}

/* Écrit un entier de 16 bits en big-endian (octet de poids fort en premier). */
static void ecrire_u16(FILE *file, uint16_t valeur) {
    ecrire_u8(file, (uint8_t)(valeur >> 8));
    ecrire_u8(file, (uint8_t)(valeur & 0xFF));
}

/* Écrit un marqueur JPEG : 0xFF suivi de son code. */
static void ecrire_marqueur(FILE *file, uint8_t code) {
    ecrire_u8(file, 0xFF);
    ecrire_u8(file, code);
}

void ecriture_APP0(FILE *file) {
    ecrire_marqueur(file, 0xE0);
    ecrire_u16(file, 16);                 /* longueur de la section */
    fwrite("JFIF", 1, 5, file);           /* identifiant "JFIF\0" */
    ecrire_u8(file, 1);                   /* version 1.1 */
    ecrire_u8(file, 1);
    ecrire_u8(file, 0);                   /* pas d'unité de densité : seul le rapport d'aspect compte */
    ecrire_u16(file, 1);                  /* densité horizontale */
    ecrire_u16(file, 1);                  /* densité verticale (rapport 1:1) */
    ecrire_u8(file, 0);                   /* pas de miniature */
    ecrire_u8(file, 0);
}

/* Écrit une section DQT contenant une seule table de quantification de 64 valeurs. */
static void ecrire_table_DQT(FILE *file, uint8_t indice, const uint8_t table[64]) {
    ecrire_marqueur(file, 0xDB);
    ecrire_u16(file, 2 + 1 + 64);         /* longueur : champ + indice + 64 valeurs */
    ecrire_u8(file, indice);              /* précision 8 bits (4 bits de poids fort à 0) + indice de table */
    fwrite(table, 1, 64, file);
}

void ecriture_DQT(FILE *file, uint8_t N) {
    ecrire_table_DQT(file, 0, quantification_table_Y);
    if (N == 3) {
        ecrire_table_DQT(file, 1, quantification_table_CbCr);
    }
}

void ecriture_SOF0(FILE *file, uint8_t N, uint16_t hauteur, uint16_t largeur, const Echantillonnage *ech) {
    ecrire_marqueur(file, 0xC0);
    ecrire_u16(file, 8 + 3 * N);          /* longueur : 8 octets fixes + 3 par composante */
    ecrire_u8(file, 8);                   /* précision : 8 bits par composante (baseline) */
    ecrire_u16(file, hauteur);
    ecrire_u16(file, largeur);
    ecrire_u8(file, N);

    for (uint8_t c = 0; c < N; c++) {
        /* Une image en niveaux de gris n'est jamais sous-échantillonnée : Y vaut 1x1. */
        const uint8_t h = (N == 1) ? 1 : ech->h[c];
        const uint8_t v = (N == 1) ? 1 : ech->v[c];
        ecrire_u8(file, c + 1);           /* identifiant de composante : 1 = Y, 2 = Cb, 3 = Cr */
        ecrire_u8(file, (uint8_t)((h << 4) | v)); /* facteurs d'échantillonnage horizontal | vertical */
        ecrire_u8(file, (c == 0) ? 0 : 1);        /* table de quantification : 0 pour Y, 1 pour Cb/Cr */
    }
}

/* Écrit une section DHT : une table de Huffman (classe 0 = DC, 1 = AC) pour une composante. */
static void ecrire_table_DHT(FILE *file, int classe, int composante) {
    const uint8_t nb_symboles = htables_nb_symbols[classe][composante];

    ecrire_marqueur(file, 0xC4);
    ecrire_u16(file, 2 + 1 + 16 + nb_symboles);  /* longueur : champ + identifiant + 16 comptes + symboles */
    ecrire_u8(file, (uint8_t)((classe << 4) | composante)); /* classe (DC/AC) | identifiant de table */
    fwrite(htables_nb_symb_per_lengths[classe][composante], 1, 16, file); /* nombre de codes de chaque longueur 1..16 */
    fwrite(htables_symbols[classe][composante], 1, nb_symboles, file);    /* symboles, par longueur de code croissante */
}

void ecriture_DHT(FILE *file, uint8_t N) {
    for (int composante = 0; composante < N; composante++) {
        ecrire_table_DHT(file, 0, composante); /* table DC */
        ecrire_table_DHT(file, 1, composante); /* table AC */
    }
}

void ecriture_SOS(FILE *file, uint8_t N) {
    ecrire_marqueur(file, 0xDA);
    ecrire_u16(file, 2 * N + 6);          /* longueur : 6 octets fixes + 2 par composante */
    ecrire_u8(file, N);

    for (uint8_t c = 0; c < N; c++) {
        ecrire_u8(file, c + 1);                   /* identifiant de composante */
        ecrire_u8(file, (uint8_t)((c << 4) | c)); /* tables de Huffman DC | AC de la composante */
    }

    ecrire_u8(file, 0);                   /* Ss : premier coefficient (0) */
    ecrire_u8(file, 63);                  /* Se : dernier coefficient (63) */
    ecrire_u8(file, 0);                   /* Ah | Al : pas d'approximation successive en baseline */
}
