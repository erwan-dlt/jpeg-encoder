/**
 * @file zigzag.h
 * @brief Parcours en zigzag d'un bloc 8x8.
 */
#ifndef zigzag_h
#define zigzag_h

#include <stdint.h>

/**
 * @brief Réordonne un bloc 8x8 en un vecteur de 64 valeurs, en zigzag.
 *
 * Le parcours part du coin haut-gauche (coefficient DC) et va vers le coin
 * bas-droit : les basses fréquences, souvent non nulles, viennent en premier, et
 * les hautes fréquences, souvent nulles après quantification, sont regroupées à la fin.
 *
 * @param matrice Bloc 8x8 (ligne, colonne).
 * @param vecteur Les 64 valeurs dans l'ordre du zigzag.
 */
void zigzag(int16_t matrice[8][8], int16_t vecteur[64]);

#endif /* zigzag_h */
