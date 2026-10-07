#include "dct.h"

/* cosinus[k][n] = cos((2n + 1) * k * pi / 16) : noyau de la DCT pour une dimension de taille 8. */
static const float cosinus[8][8] = {
    {1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000},
    {0.980785, 0.831470, 0.555570, 0.195090, -0.195090, -0.555570, -0.831470, -0.980785},
    {0.923880, 0.382683, -0.382683, -0.923880, -0.923880, -0.382683, 0.382683, 0.923880},
    {0.831470, -0.195090, -0.980785, -0.555570, 0.555570, 0.980785, 0.195090, -0.831470},
    {0.707107, -0.707107, -0.707107, 0.707107, 0.707107, -0.707107, -0.707107, 0.707107},
    {0.555570, -0.980785, 0.195090, 0.831470, -0.831470, -0.195090, 0.980785, -0.555570},
    {0.382683, -0.923880, 0.923880, -0.382683, -0.382683, 0.923880, -0.923880, 0.382683},
    {0.195090, -0.555570, 0.831470, -0.980785, 0.980785, -0.831470, 0.555570, -0.195090}
};

/* Coefficient de normalisation C(k) : 1/racine(2) pour k = 0, 1 sinon. */
static float coefficient(int k) {
    return (k == 0) ? 0.707107f : 1.0f;
}

void dct_8x8(uint8_t bloc_depart[8][8], int16_t bloc_final[8][8]) {
    float lignes[8][8];  /* résultat de la DCT 1D appliquée à chaque ligne */

    for (int x = 0; x < 8; x++) {
        for (int j = 0; j < 8; j++) {
            float somme = 0;
            for (int y = 0; y < 8; y++) {
                somme += (bloc_depart[x][y] - 128) * cosinus[j][y];
            }
            lignes[x][j] = somme;
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            float somme = 0;
            for (int x = 0; x < 8; x++) {
                somme += lignes[x][j] * cosinus[i][x];
            }
            bloc_final[i][j] = (int16_t)(0.25f * coefficient(i) * coefficient(j) * somme);
        }
    }
}
