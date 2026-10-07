/**
 * @file ecriture_entete_JPEG.h
 * @brief Écriture des sections d'en-tête d'un fichier JPEG baseline (JFIF).
 *
 * Un fichier produit par l'encodeur a la structure suivante :
 *   SOI, APP0, DQT, SOF0, DHT, SOS, <données compressées>, EOI.
 * Les entiers de 16 bits sont écrits en big-endian, comme l'impose la norme.
 */
#ifndef ecriture_entete_JPEG_h
#define ecriture_entete_JPEG_h

#include <stdint.h>
#include <stdio.h>
#include "echantillonnage.h"

/**
 * @brief Écrit la section APP0 (identification JFIF 1.1).
 * @param file Fichier JPEG ouvert en écriture binaire.
 */
void ecriture_APP0(FILE *file);

/**
 * @brief Écrit les tables de quantification (section DQT).
 * @param file Fichier JPEG.
 * @param N    1 pour une image en niveaux de gris (table de luminance seule),
 *             3 pour une image couleur (luminance puis chrominance).
 */
void ecriture_DQT(FILE *file, uint8_t N);

/**
 * @brief Écrit l'en-tête de l'image (section SOF0, baseline DCT).
 * @param file     Fichier JPEG.
 * @param N        Nombre de composantes : 1 (gris) ou 3 (couleur).
 * @param hauteur  Hauteur réelle de l'image en pixels (pas la hauteur complétée).
 * @param largeur  Largeur réelle de l'image en pixels.
 * @param ech      Facteurs d'échantillonnage de Y, Cb, Cr (seul Y est lu si N = 1).
 */
void ecriture_SOF0(FILE *file, uint8_t N, uint16_t hauteur, uint16_t largeur, const Echantillonnage *ech);

/**
 * @brief Écrit les tables de Huffman DC et AC de chaque composante (section DHT).
 * @param file Fichier JPEG.
 * @param N    Nombre de composantes : 1 ou 3.
 */
void ecriture_DHT(FILE *file, uint8_t N);

/**
 * @brief Écrit l'en-tête du balayage (section SOS), juste avant les données compressées.
 * @param file Fichier JPEG.
 * @param N    Nombre de composantes : 1 ou 3.
 */
void ecriture_SOS(FILE *file, uint8_t N);

#endif /* ecriture_entete_JPEG_h */
