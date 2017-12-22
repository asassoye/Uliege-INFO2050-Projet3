#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Comic.h"
#include "PNM.h"


/* ------------------------------------------------------------------------- *\
 * ./Projet_3 1000 1 test.pnm 01.pnm 02.pnm 03.pnm 04.pnm 05.pnm 06.pnm 07.pnm 08.pnm 09.pnm 10.pnm 11.pnm
 \* ------------------------------------------------------------------------- */


int main(int argc, char *argv[]) {


    /* --- Argument parsing --- */
    if (argc < 5) {
        fprintf(stderr, "Usage: %s width border comic.pnm image1.pnm "
                "image2.pnm ... imageN.pnm\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    size_t comicWidth = 0;
    if (sscanf(argv[1], "%zu", &comicWidth) != 1) {
        fprintf(stderr, "Aborting; Width should be an "
                "unsigned integer. Got '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    size_t comicBorder = 0;
    if (sscanf(argv[2], "%zu", &comicBorder) != 1) {
        fprintf(stderr, "Aborting; Border should be an "
                "unsigned integer. Got '%s'\n", argv[2]);
        exit(EXIT_FAILURE);
    }


    size_t nbImages = argc - 4;

    PNMImage **images = (PNMImage **) malloc((nbImages) * sizeof(PNMImage *));
    if (!images) {
        fprintf(stderr, "Aborting; unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    // Load images
    for (size_t i = 0; i < nbImages; i++) {
        images[i] = readPNM(argv[i + 4]);

        if (!images[i]) {
            fprintf(stderr, "Aborting; cannot load image '%s'\n", argv[i + 4]);
            for (size_t k = 0; k < i; k++) {
                freePNM(images[k]);
            }
            free(images);
            exit(EXIT_FAILURE);
        }
    }



    size_t *test = wrapImages((const PNMImage **) images, nbImages, comicWidth, comicBorder);

    fprintf(stderr, "Comic width: %zu\n", comicWidth);
    fprintf(stderr, "Comic boder: %zu\n", comicBorder);
    fprintf(stderr, "NB: %zu\n", nbImages);
    fprintf(stderr, "Table: ");

    for (int j = 0; j < nbImages; ++j) {
        fprintf(stderr, "%zu ", test[j]);
    }
    fprintf(stderr, "\n");

    free(images);

    exit(EXIT_SUCCESS);
}
