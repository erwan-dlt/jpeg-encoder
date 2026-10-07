#include <stdio.h>
#include <stdlib.h>
#include "echantillonnage.h"

Echantillonnage echantillonnage_par_defaut(void) {
    Echantillonnage e = {{1, 1, 1}, {1, 1, 1}};
    return e;
}

int facteur_h_max(const Echantillonnage *e, int nb_composantes) {
    int max = 1;
    for (int c = 0; c < nb_composantes; c++) {
        if (e->h[c] > max) max = e->h[c];
    }
    return max;
}

int facteur_v_max(const Echantillonnage *e, int nb_composantes) {
    int max = 1;
    for (int c = 0; c < nb_composantes; c++) {
        if (e->v[c] > max) max = e->v[c];
    }
    return max;
}

const char *echantillonnage_verifier(const Echantillonnage *e) {
    int blocs_par_mcu = 0;
    for (int c = 0; c < 3; c++) {
        if (e->h[c] < 1 || e->h[c] > FACTEUR_MAX || e->v[c] < 1 || e->v[c] > FACTEUR_MAX) {
            return "les facteurs d'échantillonnage doivent être compris entre 1 et 4";
        }
        blocs_par_mcu += e->h[c] * e->v[c];
    }
    const int hmax = facteur_h_max(e, 3);
    const int vmax = facteur_v_max(e, 3);
    for (int c = 0; c < 3; c++) {
        if (hmax % e->h[c] != 0 || vmax % e->v[c] != 0) {
            return "chaque facteur doit diviser le plus grand facteur de sa direction (ex. 2x2,1x1,1x1)";
        }
    }
    if (blocs_par_mcu > 10) {
        return "une MCU ne peut pas contenir plus de 10 blocs 8x8";
    }
    return NULL;
}

Bloc *echantillonner(MCU **mcu, int blocs_larg, int blocs_haut,
                     const Echantillonnage *e, int nb_composantes, size_t *nb_blocs) {
    const int hmax = facteur_h_max(e, nb_composantes);
    const int vmax = facteur_v_max(e, nb_composantes);
    const int mcu_larg = blocs_larg / hmax;   /* nombre de MCU JPEG en largeur */
    const int mcu_haut = blocs_haut / vmax;   /* nombre de MCU JPEG en hauteur */

    int blocs_par_mcu = 0;
    for (int c = 0; c < nb_composantes; c++) {
        blocs_par_mcu += e->h[c] * e->v[c];
    }
    *nb_blocs = (size_t)mcu_larg * mcu_haut * blocs_par_mcu;

    Bloc *resultat = malloc(*nb_blocs * sizeof(Bloc));
    if (resultat == NULL) {
        perror("Erreur lors de l'allocation des blocs à coder");
        exit(EXIT_FAILURE);
    }

    /* Les trois composantes d'une MCU à pleine résolution (au plus 32x32 pixels). */
    uint8_t plan[3][8 * FACTEUR_MAX][8 * FACTEUR_MAX];
    size_t indice = 0;

    for (int my = 0; my < mcu_haut; my++) {
        for (int mx = 0; mx < mcu_larg; mx++) {
            /* 1. Conversion RGB -> YCbCr de chaque bloc 8x8 de la MCU. */
            for (int bv = 0; bv < vmax; bv++) {
                for (int bh = 0; bh < hmax; bh++) {
                    YCbCrImage ycc = convertToYCbCr(mcu[my * vmax + bv][mx * hmax + bh].pixels);
                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                            plan[0][bv * 8 + i][bh * 8 + j] = ycc.Y.bloc[i][j];
                            plan[1][bv * 8 + i][bh * 8 + j] = ycc.Cb.bloc[i][j];
                            plan[2][bv * 8 + i][bh * 8 + j] = ycc.Cr.bloc[i][j];
                        }
                    }
                }
            }

            /* 2. Pour chaque composante : h x v blocs, chaque pixel étant la moyenne
             *    d'un rectangle de (hmax/h) x (vmax/v) pixels de la MCU. */
            for (int c = 0; c < nb_composantes; c++) {
                const int rh = hmax / e->h[c];
                const int rv = vmax / e->v[c];
                const int nb_pixels = rh * rv;
                for (int bv = 0; bv < e->v[c]; bv++) {
                    for (int bh = 0; bh < e->h[c]; bh++) {
                        Bloc *bloc = &resultat[indice++];
                        bloc->type_bloc = (uint8_t)(c + 1);
                        for (int i = 0; i < 8; i++) {
                            for (int j = 0; j < 8; j++) {
                                int somme = 0;
                                for (int dv = 0; dv < rv; dv++) {
                                    for (int dh = 0; dh < rh; dh++) {
                                        somme += plan[c][(bv * 8 + i) * rv + dv][(bh * 8 + j) * rh + dh];
                                    }
                                }
                                bloc->bloc[i][j] = (uint8_t)((somme + nb_pixels / 2) / nb_pixels);
                            }
                        }
                    }
                }
            }
        }
    }
    return resultat;
}
