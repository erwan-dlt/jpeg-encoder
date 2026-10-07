/**
 * @file gestion_bloc.h
 * @brief Codage complet d'un bloc 8x8 : DCT, zigzag, quantification, codage entropique.
 */
#ifndef gestion_bloc_h
#define gestion_bloc_h

#include <stdio.h>
#include "converterYCbCr.h"
#include "codage_ACDC_couleur.h"

/**
 * @brief Encode un bloc 8x8 d'une composante et écrit ses bits dans le fichier.
 *
 * @param bloc     Bloc de Y, Cb ou Cr (son type_bloc choisit les tables de quantification et de Huffman).
 * @param file     Fichier JPEG.
 * @param buffer   Tampon de bits partagé par tous les blocs.
 * @param valeurDC DC précédent de chaque composante.
 */
void encoder_bloc(Bloc bloc, FILE *file, buffer_taille *buffer, valDCprec *valeurDC);

#endif /* gestion_bloc_h */
