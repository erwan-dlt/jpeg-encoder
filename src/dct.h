/**
 * @file dct.h
 * @brief Transformée en cosinus discrète (DCT) d'un bloc 8x8.
 */
#ifndef dct_h
#define dct_h

#include <stdint.h>

/**
 * @brief Calcule la DCT 2D d'un bloc 8x8.
 *
 * Les pixels sont d'abord centrés autour de 0 (on retire 128), puis la transformée
 * est calculée en deux passes de DCT 1D (sur les lignes, puis sur les colonnes), ce
 * qui revient à la formule 2D de la norme avec 8 fois moins de multiplications.
 * Les coefficients sont tronqués vers zéro.
 *
 * @param bloc_depart Pixels d'une composante (valeurs de 0 à 255).
 * @param bloc_final  Coefficients de la DCT : [0][0] est le DC, les autres sont les AC.
 */
void dct_8x8(uint8_t bloc_depart[8][8], int16_t bloc_final[8][8]);

#endif /* dct_h */
