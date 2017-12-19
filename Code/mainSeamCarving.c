/* ------------------------------------------------------------------------- *\
 * NOM
 *      seam_carving
 * SYNOPSIS
 *      seam_carving input_file output_file nbPix
 * DESCIRPTION
 *      Apply the Seam Carving algorithm to the given input image
 * ARGUMENTS
 *      input_file      An input image file in PNM format
 *      output_file     An output image file (format will be PNM)
 *      nbPix          The number of pixel (integer) by which
 *                      to increase (nbPix > 0) or decrease (nbPix < 0)
 *                      the input image
 * USAGE
 *      ./mainSeamCarving input.pnm output.pnm 100
 *          will ouput an image whose width is 100 pixels more than the input
 *      ./mainSeamCarving input.pnm output.pnm -50
 *          will ouput an image whose width is 50 pixels less than the input
 \* ------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SeamCarving.h"
#include "PNM.h"


int main(int argc, char* argv[])
{
    /* --- Argument parsing --- */
    if (argc != 4) {
        fprintf(stderr, "Usage: %s input.pnm output.pnm nbPix\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Carving width
    int nbPix;
    if(sscanf (argv[3], "%d", &nbPix) != 1)
    {
        fprintf(stderr, "%s\n", "Aborting; nbPix should be an integer.\n");
        exit(EXIT_FAILURE);
    }

    // Load image
    PNMImage* original = readPNM(argv[1]);
    if (!original)
    {
        fprintf(stderr, "Aborting; cannot load image '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* --- Seam Carving --- */
    PNMImage* output;

    if (nbPix < 0)
    {
        output = reduceImageWidth(original, (size_t) -nbPix);
    } else {
        output = increaseImageWidth(original, (size_t) nbPix);
    }

    /* --- Writing output --- */
    if (!output)
    {
        fprintf(stderr, "Aborting; cannot build new image\n");
        freePNM(original);
        exit(EXIT_FAILURE);
    }

    // Save and free
    writePNM(argv[2], output);
    freePNM(original);
    freePNM(output);

    exit(EXIT_SUCCESS);
}
