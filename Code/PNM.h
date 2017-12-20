/* ------------------------------------------------------------------------- *
 * PNM.
 * Interface for loading and writing PNM images.
 *
 * Partly adapted from http://stackoverflow.com/a/2699908
 * ------------------------------------------------------------------------- */

#ifndef _PNM_H_
#define _PNM_H_

#include <stddef.h>


// Types ----------------------------------------------------------------------

typedef struct {
    unsigned char red, green, blue;
} PNMPixel;

typedef struct {
    size_t width;
    size_t height;
    PNMPixel *data;     // Pixel (i, j) is at position i * width + j
} PNMImage;


// Methods --------------------------------------------------------------------

/* ------------------------------------------------------------------------- *
 * Create an empty PNM image.
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * width        Width of the image (in pixels)
 * height       Height of the image (in pixels)
 *
 * RETURN
 * image        Pointer to an empty PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage *createPNM(size_t width, size_t height);

/* ------------------------------------------------------------------------- *
 * Free a PNM image.
 *
 * PARAMETER
 * image        Pointer to a PNM image
 * ------------------------------------------------------------------------- */
void freePNM(PNMImage *image);

/* ------------------------------------------------------------------------- *
 * Load a PNM image from a file.
 * The PNM image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * filename     Path to the PNM file
 *
 * RETURN
 * image        Pointer to the loaded PNM image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage *readPNM(const char *filename);

/* ------------------------------------------------------------------------- *
 * Write a PNM image into a file.
 *
 * PARAMETERS
 * filename     Path to the PNM file
 * image        Pointer to the PNM image to write
 *
 * RETURN
 * 0            In case of success
 * -1           Otherwise
 * ------------------------------------------------------------------------- */
int writePNM(const char *filename, const PNMImage *image);

#endif // _PNM_H_
