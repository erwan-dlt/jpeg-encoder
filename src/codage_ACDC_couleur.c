#include <stdio.h>
#include <stdlib.h>
#include "codage_ACDC_couleur.h"
#include "tableaux_huffman.h"

/* Symboles AC spéciaux de la norme. */
#define SYMBOLE_EOB 0x00  /* fin de bloc : tous les coefficients restants sont nuls */
#define SYMBOLE_ZRL 0xF0  /* 16 zéros consécutifs */

/* Écrit les octets complets du tampon ; chaque 0xFF est suivi d'un 0x00 (byte stuffing). */
static void ecriture_buffer(buffer_taille *buffer, FILE *file) {
    while (buffer->taille >= 8) {
        uint8_t octet = (buffer->tampon >> (buffer->taille - 8)) & 0xFF;
        fwrite(&octet, 1, 1, file);
        if (octet == 0xFF) {
            const uint8_t zero = 0x00;
            fwrite(&zero, 1, 1, file);
        }
        buffer->taille -= 8;
        buffer->tampon &= (1ULL << buffer->taille) - 1;  /* ne garde que les bits non écrits */
    }
}

/* Ajoute les `nb_bits` bits de poids faible de `valeur` au tampon, puis écrit les octets complets. */
static void ajouter_bits(buffer_taille *buffer, FILE *file, uint16_t valeur, uint16_t nb_bits) {
    buffer->tampon = (buffer->tampon << nb_bits) | valeur;
    buffer->taille += nb_bits;
    ecriture_buffer(buffer, file);
}

/* Écrit le code de Huffman de `symbole`. Chaque ligne de table vaut {symbole, code, longueur}. */
static void ecrire_symbole(buffer_taille *buffer, FILE *file, uint16_t (*table)[3],
                           size_t taille_table, uint8_t symbole) {
    for (size_t i = 0; i < taille_table; i++) {
        if (table[i][0] == symbole) {
            ajouter_bits(buffer, file, table[i][1], table[i][2]);
            return;
        }
    }
    fprintf(stderr, "Erreur : symbole 0x%02X absent de la table de Huffman\n", symbole);
    exit(EXIT_FAILURE);
}

/* Magnitude d'une valeur : nombre de bits de sa valeur absolue (0 pour 0). */
static uint16_t magnitude(int16_t valeur) {
    if (valeur < 0) valeur = -valeur;
    uint16_t m = 0;
    while (valeur != 0) {
        m++;
        valeur >>= 1;
    }
    return m;
}

/*
 * Représentation d'une valeur sur `magnitude` bits : la valeur elle-même si elle est
 * positive, sinon son complément à un (valeur + 2^magnitude - 1).
 */
static uint16_t indice(int16_t valeur, uint16_t magnitude) {
    if (valeur >= 0) {
        return valeur;
    }
    return valeur + (1 << magnitude) - 1;
}

/* Code la différence DC : code de Huffman de la magnitude, puis les bits de la valeur. */
static void compression_DC(int16_t difference, buffer_taille *buffer, FILE *file, uint16_t (*table)[3]) {
    const uint16_t m = magnitude(difference);
    ecrire_symbole(buffer, file, table, 12, (uint8_t)m);
    ajouter_bits(buffer, file, indice(difference, m), m);
}

/* Code un coefficient AC non nul précédé de `zeros` zéros : symbole (zéros << 4 | magnitude), puis la valeur. */
static void compression_AC(int16_t valeur, uint16_t zeros, buffer_taille *buffer, FILE *file, uint16_t (*table)[3]) {
    const uint16_t m = magnitude(valeur);
    ecrire_symbole(buffer, file, table, 162, (uint8_t)((zeros << 4) | m));
    ajouter_bits(buffer, file, indice(valeur, m), m);
}

void compression_ACDC(int16_t vecteur[64], FILE *file, buffer_taille *buffer,
                      int16_t *valueDCprec, uint8_t type_bloc) {
    uint16_t (*tab_DC)[3];
    uint16_t (*tab_AC)[3];

    if (type_bloc == 1) {
        tab_DC = tab_DC_Y;
        tab_AC = tab_AC_Y;
    } else if (type_bloc == 2) {
        tab_DC = tab_DC_Cb;
        tab_AC = tab_AC_Cb;
    } else if (type_bloc == 3) {
        tab_DC = tab_DC_Cr;
        tab_AC = tab_AC_Cr;
    } else {
        fprintf(stderr, "Erreur : type de bloc inconnu (%d)\n", type_bloc);
        exit(EXIT_FAILURE);
    }

    /* Coefficient DC : différence avec le DC du bloc précédent de la même composante. */
    compression_DC(vecteur[0] - *valueDCprec, buffer, file, tab_DC);
    *valueDCprec = vecteur[0];

    /* Indice du dernier coefficient AC non nul (0 s'il n'y en a aucun). */
    int dernier = 63;
    while (dernier > 0 && vecteur[dernier] == 0) {
        dernier--;
    }

    /* Coefficients AC : on compte les zéros qui précèdent chaque valeur non nulle. */
    uint16_t zeros = 0;
    for (int i = 1; i <= dernier; i++) {
        if (vecteur[i] != 0) {
            compression_AC(vecteur[i], zeros, buffer, file, tab_AC);
            zeros = 0;
        } else {
            zeros++;
            if (zeros == 16) {  /* au plus 15 zéros par symbole : on en émet 16 d'un coup */
                ecrire_symbole(buffer, file, tab_AC, 162, SYMBOLE_ZRL);
                zeros = 0;
            }
        }
    }

    /* Les coefficients restants sont tous nuls : fin de bloc (inutile si le bloc est plein). */
    if (dernier < 63) {
        ecrire_symbole(buffer, file, tab_AC, 162, SYMBOLE_EOB);
    }
}

void vider_buffer(buffer_taille *buffer, FILE *file) {
    if (buffer->taille > 0) {
        const uint16_t bits_libres = 8 - buffer->taille;
        const uint8_t octet = (uint8_t)((buffer->tampon << bits_libres) | ((1u << bits_libres) - 1));
        fwrite(&octet, 1, 1, file);
        if (octet == 0xFF) {
            const uint8_t zero = 0x00;
            fwrite(&zero, 1, 1, file);
        }
        buffer->tampon = 0;
        buffer->taille = 0;
    }
}
