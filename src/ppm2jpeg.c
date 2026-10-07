/**
 * @file ppm2jpeg.c
 * @brief Encodeur JPEG baseline : convertit une image PGM/PPM en fichier JPEG.
 *
 * Chaîne de traitement :
 *   lecture de l'image -> découpage en blocs 8x8 -> conversion RGB->YCbCr et
 *   sous-échantillonnage -> pour chaque bloc : DCT, zigzag, quantification,
 *   codage DC/AC et Huffman -> écriture du fichier JPEG.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arguments.h"
#include "codage_ACDC_couleur.h"
#include "echantillonnage.h"
#include "ecriture_entete_JPEG.h"
#include "gestion_bloc.h"
#include "lecture_fichier_entree.h"
#include "MCU.h"

int main(int argc, char *argv[]) {
    Options options;
    analyser_arguments(argc, argv, &options);

    /* 1. Lecture de l'image, complétée pour former un nombre entier de MCU. */
    const int hmax = facteur_h_max(&options.echantillonnage, 3);
    const int vmax = facteur_v_max(&options.echantillonnage, 3);
    FileData image = spec_image(options.fichier_entree, hmax, vmax);

    const int est_couleur = (strcmp(image.type_file, "P6") == 0);
    const uint8_t nb_composantes = est_couleur ? 3 : 1;
    if (!est_couleur && options.echantillonnage_demande) {
        fprintf(stderr, "Avertissement : --sample est ignoré pour une image en niveaux de gris\n");
    }
    const Echantillonnage echantillonnage = est_couleur ? options.echantillonnage : echantillonnage_par_defaut();

    /* 2. Découpage en blocs 8x8, conversion YCbCr et sous-échantillonnage. */
    MCU **blocs_rgb = decoupe_en_blocs_8x8(image);
    size_t nb_blocs;
    Bloc *blocs = echantillonner(blocs_rgb, image.largeur / 8, image.hauteur / 8,
                                 &echantillonnage, nb_composantes, &nb_blocs);

    /* 3. Écriture de l'en-tête JPEG. */
    FILE *file = fopen(options.fichier_sortie, "wb");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier de sortie");
        return EXIT_FAILURE;
    }
    fwrite("\xFF\xD8", 1, 2, file);  /* SOI : début de l'image */
    ecriture_APP0(file);
    ecriture_DQT(file, nb_composantes);
    ecriture_SOF0(file, nb_composantes, (uint16_t)image.hauteur_originale,
                  (uint16_t)image.largeur_originale, &echantillonnage);
    ecriture_DHT(file, nb_composantes);
    ecriture_SOS(file, nb_composantes);

    /* 4. Codage des blocs, dans l'ordre des MCU. */
    buffer_taille buffer = {0, 0};
    valDCprec valeurs_DC = {0, 0, 0};
    for (size_t i = 0; i < nb_blocs; i++) {
        encoder_bloc(blocs[i], file, &buffer, &valeurs_DC);
    }
    vider_buffer(&buffer, file);

    fwrite("\xFF\xD9", 1, 2, file);  /* EOI : fin de l'image */
    if (fclose(file) != 0) {
        perror("Erreur lors de l'écriture du fichier de sortie");
        return EXIT_FAILURE;
    }

    /* 5. Libération de la mémoire. */
    free(blocs);
    freeMCU(blocs_rgb, image.hauteur / 8);
    freeDonneesRGB(&image);
    liberer_options(&options);
    return EXIT_SUCCESS;
}
