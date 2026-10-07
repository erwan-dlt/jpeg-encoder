#include <stdio.h>
#include <stdlib.h>
#include "MCU.h"

void freeMCU(MCU **blocs, int mcu_hauteur) {
    for (int i = 0; i < mcu_hauteur; i++) {
        free(blocs[i]);
    }
    free(blocs);
}

MCU **decoupe_en_blocs_8x8(FileData donnees_RGB) {
    const int mcu_largeur = donnees_RGB.largeur / 8;
    const int mcu_hauteur = donnees_RGB.hauteur / 8;

    /* Allocation du tableau de lignes de blocs. */
    MCU **blocs = malloc(mcu_hauteur * sizeof(MCU *));
    if (blocs == NULL) {
        perror("Erreur lors de l'allocation mémoire pour les lignes de blocs 8x8");
        exit(EXIT_FAILURE);
    }

    /* Allocation de chaque ligne de blocs (en libérant le déjà-alloué en cas d'échec). */
    for (int i = 0; i < mcu_hauteur; i++) {
        blocs[i] = malloc(mcu_largeur * sizeof(MCU));
        if (blocs[i] == NULL) {
            perror("Erreur lors de l'allocation mémoire pour les blocs 8x8");
            for (int k = 0; k < i; k++) {
                free(blocs[k]);
            }
            free(blocs);
            exit(EXIT_FAILURE);
        }
    }

    /* Recopie des pixels dans les blocs. */
    for (int i = 0; i < mcu_hauteur; i++) {
        for (int j = 0; j < mcu_largeur; j++) {
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    blocs[i][j].pixels[x][y] = donnees_RGB.donnees[i * 8 + x][j * 8 + y];
                }
            }
        }
    }
    return blocs;
}
