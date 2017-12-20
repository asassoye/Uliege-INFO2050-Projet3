/* ------------------------------------------------------------------------- *
 * Implementation of the PNM interface
 *
 * Partly adapted from http://stackoverflow.com/a/2699908
 * ------------------------------------------------------------------------- */
#include <stdlib.h>
#include <stdio.h>

#include "PNM.h"

// Methods

PNMImage *createPNM(size_t width, size_t height) {
    PNMImage *image = (PNMImage *) malloc(sizeof(PNMImage));
    if (!image) {
        return NULL;
    }

    image->width = width;
    image->height = height;

    image->data = (PNMPixel *) malloc(width * height * sizeof(PNMPixel));
    if (!image->data) {
        free(image);
        return NULL;
    }

    return image;
}

void freePNM(PNMImage *image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

PNMImage *readPNM(const char *filename) {
    char buffer[16];
    int c;

    // Open PNM file for reading
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return NULL;
    }

    // Read image format
    if (!fgets(buffer, sizeof(buffer), fp)) {
        return NULL;
    }

    if (buffer[0] != 'P' || buffer[1] != '6') {
        return NULL;
    }

    // Check for comments
    c = getc(fp);
    while (c == '#') {
        while (getc(fp) != '\n');
        c = getc(fp);
    }

    ungetc(c, fp);

    // Read image size
    size_t width;
    size_t height;

    if (fscanf(fp, "%zu %zu", &width, &height) != 2) {
        return NULL;
    }

    // Read RGB depth
    if (fscanf(fp, "%d", &c) != 1) {
        return NULL;
    }

    if (c != 255) {
        return NULL;
    }

    while (fgetc(fp) != '\n');

    // Allocate memory
    PNMImage *image = createPNM(width, height);
    if (!image) {
        return NULL;
    }

    // Read pixels
    if (fread(image->data, 3 * image->width,
              image->height, fp) != image->height) {
        freePNM(image);
        return NULL;
    }

    fclose(fp);
    return image;
}

int writePNM(const char *filename, const PNMImage *image) {
    // Open file
    FILE *fp;

    fp = fopen(filename, "wb");
    if (!fp) {
        return -1;
    }

    // Write content
    fprintf(fp, "P6\n");
    fprintf(fp, "%zu %zu\n", image->width, image->height);
    fprintf(fp, "255\n");
    fwrite(image->data, 3 * image->width, image->height, fp);

    fclose(fp);

    return 0;
}
