/**
 * @file codage_ACDC_couleur.h
 * @brief Codage entropique d'un bloc quantifié : coefficient DC différentiel,
 *        coefficients AC en RLE, codes de Huffman et écriture du flux de bits.
 */
#ifndef codage_ACDC_couleur_h
#define codage_ACDC_couleur_h

#include <stdint.h>
#include <stdio.h>

/** Dernière valeur DC de chaque composante (le DC est codé comme une différence). */
typedef struct valDCprec {
    int16_t Y_DC;
    int16_t Cb_DC;
    int16_t Cr_DC;
} valDCprec;

/** Tampon de bits en attente d'écriture : les `taille` bits de poids faible de `tampon`. */
typedef struct {
    uint64_t tampon;
    uint16_t taille;
} buffer_taille;

/**
 * @brief Code un bloc de 64 coefficients quantifiés (ordre zigzag) dans le flux.
 *
 * Écrit la différence entre le DC du bloc et le DC précédent de la composante,
 * puis les coefficients AC sous forme (nombre de zéros, valeur), avec les symboles
 * spéciaux ZRL (16 zéros) et EOB (fin de bloc). Les octets 0xFF du flux sont suivis
 * d'un 0x00 (« byte stuffing »), comme l'impose la norme.
 *
 * @param vecteur     Les 64 coefficients du bloc.
 * @param file        Fichier JPEG.
 * @param buffer      Tampon de bits partagé par tous les blocs.
 * @param valueDCprec DC précédent de la composante ; mis à jour par la fonction.
 * @param type_bloc   1 pour Y, 2 pour Cb, 3 pour Cr (choisit les tables de Huffman).
 */
void compression_ACDC(int16_t vecteur[64], FILE *file, buffer_taille *buffer,
                      int16_t *valueDCprec, uint8_t type_bloc);

/**
 * @brief Écrit les bits restants du tampon, en complétant l'octet avec des 1
 *        (norme JPEG). À appeler une fois, après le dernier bloc.
 */
void vider_buffer(buffer_taille *buffer, FILE *file);

#endif /* codage_ACDC_couleur_h */
