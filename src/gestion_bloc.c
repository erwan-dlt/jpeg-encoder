#include "gestion_bloc.h"
#include "dct.h"
#include "zigzag.h"
#include "quantification.h"
#include "../include/qtables.h"

void encoder_bloc(Bloc bloc, FILE *file, buffer_taille *buffer, valDCprec *valeurDC) {
    int16_t blocDCT[8][8];
    int16_t vecteur[64];

    dct_8x8(bloc.bloc, blocDCT);   /* 1. transformée en cosinus discrète */
    zigzag(blocDCT, vecteur);      /* 2. parcours en zigzag : basses fréquences d'abord */

    /* 3. quantification (luminance ou chrominance) puis codage entropique */
    if (bloc.type_bloc == 1) {
        quantif(vecteur, quantification_table_Y);
        compression_ACDC(vecteur, file, buffer, &valeurDC->Y_DC, bloc.type_bloc);
    } else if (bloc.type_bloc == 2) {
        quantif(vecteur, quantification_table_CbCr);
        compression_ACDC(vecteur, file, buffer, &valeurDC->Cb_DC, bloc.type_bloc);
    } else if (bloc.type_bloc == 3) {
        quantif(vecteur, quantification_table_CbCr);
        compression_ACDC(vecteur, file, buffer, &valeurDC->Cr_DC, bloc.type_bloc);
    }
}
