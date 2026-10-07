/**
 * @file MCU.h
 * @brief Découpage de l'image en blocs de 8x8 pixels RGB.
 *
 * Remarque : le type s'appelle MCU pour des raisons historiques, mais il désigne
 * un bloc 8x8. L'unité de codage JPEG (la vraie MCU) regroupe plusieurs de ces
 * blocs selon l'échantillonnage : voir echantillonnage.h.
 */
#ifndef MCU_H
#define MCU_H

#include "lecture_fichier_entree.h"

/** Bloc de 8x8 pixels RGB. */
typedef struct {
    RGB pixels[8][8];
} MCU;

/**
 * @brief Libère le tableau de blocs créé par decoupe_en_blocs_8x8.
 * @param blocs       Tableau 2D de blocs.
 * @param mcu_hauteur Nombre de lignes de blocs.
 */
void freeMCU(MCU **blocs, int mcu_hauteur);

/**
 * @brief Découpe l'image en blocs de 8x8 pixels.
 *
 * @param donnees_RGB Image dont la largeur et la hauteur sont des multiples de 8.
 * @return Tableau 2D de blocs : blocs[ligne][colonne], (hauteur/8) x (largeur/8).
 */
MCU **decoupe_en_blocs_8x8(FileData donnees_RGB);

#endif /* MCU_H */
