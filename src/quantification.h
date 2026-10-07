/**
 * @file quantification.h
 * @brief Quantification des coefficients de la DCT.
 */
#ifndef quantification_h
#define quantification_h

#include <stdint.h>

/**
 * @brief Divise chaque coefficient par la valeur correspondante de la table de quantification.
 *
 * C'est l'étape qui perd de l'information (et fait gagner de la place) : les
 * coefficients de haute fréquence, divisés par de grandes valeurs, tombent à 0.
 * La division entière tronque vers zéro.
 *
 * @param vecteur       Les 64 coefficients, en ordre zigzag ; modifié sur place.
 * @param table_quantif Table de quantification, dans le même ordre zigzag.
 */
void quantif(int16_t vecteur[64], const uint8_t table_quantif[64]);

#endif /* quantification_h */
