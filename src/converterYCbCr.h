/**
 * @file converterYCbCr.h
 * @brief Conversion d'un bloc de pixels RGB 8x8 vers l'espace YCbCr.
 */
#ifndef CONVERTER_TO_YCBCR_H
#define CONVERTER_TO_YCBCR_H

#include <stdint.h>
#include "lecture_fichier_entree.h"

/** Bloc 8x8 d'une composante, avec son type pour choisir les tables de codage. */
typedef struct {
    uint8_t bloc[8][8];
    uint8_t type_bloc; /* 1 pour Y, 2 pour Cb, 3 pour Cr */
} Bloc;

/** Les trois composantes d'un bloc de pixels, à pleine résolution. */
typedef struct {
    Bloc Y;
    Bloc Cb;
    Bloc Cr;
} YCbCrImage;

/**
 * @brief Convertit un bloc 8x8 de pixels RGB en YCbCr (formules de la norme JFIF).
 *
 * Les résultats sont arrondis puis bornés à [0, 255] : une couleur pure comme
 * (0, 0, 255) donnerait sinon Cb = 256, qui déborderait d'un octet.
 */
YCbCrImage convertToYCbCr(RGB MCU[8][8]);

#endif /* CONVERTER_TO_YCBCR_H */
