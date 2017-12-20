/* ------------------------------------------------------------------------- *
 * Comic.
 * Interface for packing a comics.
 * ------------------------------------------------------------------------- */

#ifndef _COMICS_H_
#define _COMICS_H_

#include <stddef.h>
#include "PNM.h"


/* ------------------------------------------------------------------------- *
 * Compute the optimal positions of the images on the page.
 *
 * The returned array must later be deleted by calling free().
 *
 * PARAMETERS
 * images       An array of images
 * nbImages     The number of images
 * comicWidth   The width of a page
 * comicBorder  The border around images
 *
 * RETURN
 * positions    An array of size nbImages, such that positions[i] corresponds
 *              to the row (starting from 0) of images[i] in the optimal
 *              configuration.
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
size_t *wrapImages(const PNMImage **images, size_t nbImages, size_t comicWidth,
                   size_t comicBorder);

/* ------------------------------------------------------------------------- *
 * Pack images into a single comic image.
 *
 * The returned image must later be deleted by calling freePNM().
 *
 * PARAMETERS
 * images       An array of images
 * nbImages     The number of images
 * comicWidth   The width of a page
 * comicBorder  The border around images
 *
 * RETURN
 * image        The comic image
 * NULL         if an error occured
 * ------------------------------------------------------------------------- */
PNMImage *packComic(const PNMImage **images, size_t nbImages, size_t comicWidth,
                    size_t comicBorder);

#endif // _COMICS_H_
