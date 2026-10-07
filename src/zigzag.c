#include "zigzag.h"

void zigzag(int16_t matrice[8][8], int16_t vecteur[64]) {
    int ligne = 0, col = 0;
    int dir = 1;  /* 1 : on monte vers le haut-droit ; -1 : on descend vers le bas-gauche */

    for (int k = 0; k < 64; k++) {
        vecteur[k] = matrice[ligne][col];
        if (dir == 1) {
            if (col == 7) {          /* bord droit : on descend d'une ligne */
                ligne++;
                dir = -1;
            } else if (ligne == 0) { /* bord haut : on avance d'une colonne */
                col++;
                dir = -1;
            } else {
                ligne--;
                col++;
            }
        } else {
            if (ligne == 7) {        /* bord bas : on avance d'une colonne */
                col++;
                dir = 1;
            } else if (col == 0) {   /* bord gauche : on descend d'une ligne */
                ligne++;
                dir = 1;
            } else {
                ligne++;
                col--;
            }
        }
    }
}
