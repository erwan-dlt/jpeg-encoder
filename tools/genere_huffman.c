/**
 * @file genere_huffman.c
 * @brief Génère src/tableaux_huffman.c à partir des tables de Huffman de la norme.
 *
 * include/htables.h donne, pour chaque table, le nombre de codes de chaque longueur
 * et la liste des symboles. Les codes sont obtenus avec la construction canonique de
 * la norme (annexe C) : on parcourt les longueurs de 1 à 16 ; les codes d'une même
 * longueur sont consécutifs, et le premier code d'une longueur est le suivant du
 * dernier code de la longueur précédente, décalé d'un bit vers la gauche.
 *
 * Utilisation :  make tables   (régénère src/tableaux_huffman.c)
 */
#include <stdint.h>
#include <stdio.h>
#include "../include/htables.h"

/* Affiche `code` sur `longueur` bits, en binaire (sans préfixe). */
static void afficher_binaire(uint16_t code, uint8_t longueur) {
    for (int bit = longueur - 1; bit >= 0; bit--) {
        putchar(((code >> bit) & 1) ? '1' : '0');
    }
}

int main(void) {
    /* Même ordre que dans htables.h : d'abord les tables DC, puis les AC, pour Y, Cb, Cr. */
    const char *noms[NB_SAMPLE_TYPES][NB_COLOR_COMPONENTS] = {
        {"tab_DC_Y[12][3]", "tab_DC_Cb[12][3]", "tab_DC_Cr[12][3]"},
        {"tab_AC_Y[162][3]", "tab_AC_Cb[162][3]", "tab_AC_Cr[162][3]"}
    };

    printf("#include \"tableaux_huffman.h\"\n\n");

    for (int type = 0; type < NB_SAMPLE_TYPES; type++) {
        for (int composante = 0; composante < NB_COLOR_COMPONENTS; composante++) {
            const uint8_t *symboles = htables_symbols[type][composante];
            const uint8_t *nb_par_longueur = htables_nb_symb_per_lengths[type][composante];

            printf("uint16_t %s = { ", noms[type][composante]);
            uint16_t code = 0;
            int indice_symbole = 0;
            for (int longueur = 1; longueur <= 16; longueur++) {
                for (int i = 0; i < nb_par_longueur[longueur - 1]; i++) {
                    printf("{0x%02X, 0b", symboles[indice_symbole++]);
                    afficher_binaire(code, (uint8_t)longueur);
                    printf(", %d}, ", longueur);
                    code++;
                }
                code <<= 1;
            }
            printf("};\n");
        }
    }
    return 0;
}
