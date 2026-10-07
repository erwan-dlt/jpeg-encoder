#include "converterYCbCr.h"

/* Arrondit à l'entier le plus proche et borne le résultat à [0, 255]. */
static uint8_t arrondi_borne(double valeur) {
    valeur += 0.5;
    if (valeur < 0) return 0;
    if (valeur > 255) return 255;
    return (uint8_t)valeur;
}

YCbCrImage convertToYCbCr(RGB MCU[8][8]) {
    YCbCrImage image;
    image.Y.type_bloc = 1;
    image.Cb.type_bloc = 2;
    image.Cr.type_bloc = 3;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            const double r = MCU[i][j].R;
            const double g = MCU[i][j].G;
            const double b = MCU[i][j].B;
            image.Y.bloc[i][j]  = arrondi_borne(0.299 * r + 0.587 * g + 0.114 * b);
            image.Cb.bloc[i][j] = arrondi_borne(-0.1687 * r - 0.3313 * g + 0.5 * b + 128);
            image.Cr.bloc[i][j] = arrondi_borne(0.5 * r - 0.4187 * g - 0.0813 * b + 128);
        }
    }
    return image;
}
