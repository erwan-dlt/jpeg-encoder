/**
 * @file echantillonnage.h
 * @brief Sous-échantillonnage des chrominances et assemblage des blocs en MCU.
 *
 * Une MCU (Minimum Coded Unit) JPEG regroupe, pour chaque composante (Y, Cb, Cr),
 * un rectangle de h x v blocs 8x8 : ce sont les « facteurs d'échantillonnage ».
 * Exemples :  1x1,1x1,1x1 = 4:4:4 (aucune perte de chrominance)
 *             2x1,1x1,1x1 = 4:2:2 (chrominances divisées par 2 horizontalement)
 *             2x2,1x1,1x1 = 4:2:0 (chrominances divisées par 2 dans les deux sens)
 */
#ifndef ECHANTILLONNAGE_H
#define ECHANTILLONNAGE_H

#include <stddef.h>
#include <stdint.h>
#include "MCU.h"
#include "converterYCbCr.h"

/** Plus grand facteur d'échantillonnage accepté (limite de la norme JPEG). */
#define FACTEUR_MAX 4

/** Facteurs d'échantillonnage horizontaux (h) et verticaux (v) de Y, Cb, Cr. */
typedef struct {
    uint8_t h[3];
    uint8_t v[3];
} Echantillonnage;

/** @return L'échantillonnage 1x1,1x1,1x1 (4:4:4). */
Echantillonnage echantillonnage_par_defaut(void);

/** @return Le plus grand facteur horizontal parmi les @p nb_composantes premières composantes. */
int facteur_h_max(const Echantillonnage *e, int nb_composantes);

/** @return Le plus grand facteur vertical parmi les @p nb_composantes premières composantes. */
int facteur_v_max(const Echantillonnage *e, int nb_composantes);

/**
 * @brief Vérifie qu'un échantillonnage est utilisable par l'encodeur.
 *
 * Règles : facteurs entre 1 et FACTEUR_MAX, chaque facteur divise le maximum de sa
 * direction (pour que le sous-échantillonnage soit une moyenne de pixels entiers),
 * et au plus 10 blocs par MCU (limite de la norme).
 *
 * @return NULL si l'échantillonnage est valide, sinon un message d'erreur.
 */
const char *echantillonnage_verifier(const Echantillonnage *e);

/**
 * @brief Convertit l'image en YCbCr, sous-échantillonne les chrominances et renvoie
 *        les blocs 8x8 dans l'ordre où ils doivent être codés.
 *
 * Pour chaque MCU, l'ordre est celui de la norme : tous les blocs de Y (ligne par
 * ligne), puis ceux de Cb, puis ceux de Cr. Un pixel sous-échantillonné est la
 * moyenne arrondie des pixels qu'il remplace.
 *
 * @param mcu            Blocs 8x8 de pixels RGB (voir decoupe_en_blocs_8x8).
 * @param blocs_larg     Nombre de blocs 8x8 en largeur (multiple du facteur horizontal max).
 * @param blocs_haut     Nombre de blocs 8x8 en hauteur (multiple du facteur vertical max).
 * @param e              Facteurs d'échantillonnage (déjà vérifiés).
 * @param nb_composantes 1 pour une image en niveaux de gris, 3 pour une image couleur.
 * @param nb_blocs       [sortie] Nombre de blocs du tableau retourné.
 * @return Tableau alloué dynamiquement (à libérer avec free) de *nb_blocs blocs.
 */
Bloc *echantillonner(MCU **mcu, int blocs_larg, int blocs_haut,
                     const Echantillonnage *e, int nb_composantes, size_t *nb_blocs);

#endif /* ECHANTILLONNAGE_H */
