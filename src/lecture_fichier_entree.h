/**
 * @file lecture_fichier_entree.h
 * @brief Lecture des images PGM (P5, niveaux de gris) et PPM (P6, couleur) binaires.
 */
#ifndef lecture_fichier_entree_h
#define lecture_fichier_entree_h

#include <stdint.h>

/** Un pixel RGB. Pour une image en niveaux de gris, R = G = B. */
typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} RGB;

/** Image lue en mémoire. */
typedef struct {
    char type_file[3];       /* "P5" (gris) ou "P6" (couleur) */
    int largeur;             /* largeur après recopie des bords (multiple de la taille de MCU) */
    int hauteur;             /* hauteur après recopie des bords */
    int largeur_originale;   /* largeur réelle de l'image, écrite dans l'en-tête JPEG */
    int hauteur_originale;   /* hauteur réelle de l'image */
    int intensite;           /* valeur maximale d'un canal (255) */
    RGB **donnees;           /* donnees[ligne][colonne], hauteur x largeur pixels */
} FileData;

/**
 * @brief Libère la mémoire d'une image lue par spec_image.
 * @param donnees_RGB Image à libérer.
 */
void freeDonneesRGB(FileData *donnees_RGB);

/**
 * @brief Lit une image PGM/PPM binaire 8 bits (les commentaires '#' de l'en-tête sont acceptés).
 *
 * Si les dimensions ne sont pas des multiples de la taille d'une MCU, la dernière
 * ligne et la dernière colonne sont recopiées pour compléter l'image ; les
 * dimensions d'origine restent disponibles dans largeur_originale/hauteur_originale.
 * Quitte le programme avec un message d'erreur si le fichier est invalide.
 *
 * @param nom_fichier Chemin de l'image.
 * @param facteur_h   Facteur d'échantillonnage horizontal maximal (1 si pas de sous-échantillonnage).
 * @param facteur_v   Facteur d'échantillonnage vertical maximal.
 *                    Ignorés (traités comme 1) pour une image en niveaux de gris.
 * @return L'image lue.
 */
FileData spec_image(const char *nom_fichier, int facteur_h, int facteur_v);

#endif /* lecture_fichier_entree_h */
