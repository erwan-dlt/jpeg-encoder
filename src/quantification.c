#include "quantification.h"

void quantif(int16_t vecteur[64], const uint8_t table_quantif[64]) {
    for (int k = 0; k < 64; k++) {
        vecteur[k] = vecteur[k] / (int16_t)table_quantif[k];
    }
}
