/**
 * @file tableaux_huffman.h
 * @brief Codes de Huffman standard de la norme JPEG, par composante.
 *
 * Chaque ligne vaut {symbole, code, longueur du code en bits}. Les DC ont 12
 * symboles (magnitudes 0 à 11), les AC 162 symboles (zéros << 4 | magnitude, plus
 * EOB et ZRL). Les codes sont déduits des tables de include/htables.h par le
 * générateur src/genere_huffman.c.
 */
#ifndef tableaux_huffman_h
#define tableaux_huffman_h

#include <stdint.h>

extern uint16_t tab_DC_Y[12][3];
extern uint16_t tab_DC_Cb[12][3];
extern uint16_t tab_DC_Cr[12][3];
extern uint16_t tab_AC_Y[162][3];
extern uint16_t tab_AC_Cb[162][3];
extern uint16_t tab_AC_Cr[162][3];

#endif /* tableaux_huffman_h */
