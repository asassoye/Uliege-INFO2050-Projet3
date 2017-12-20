//
// Created by Noémie Lecocq on 20/12/17.
//

//Je fais ça du coup <3
#include <stdio.h>
#include <stdlib.h>

#include "SeamCarving.h"
#include "PNM.h"

int **computeEnergy(int **energy, const PNMImage *image) {
    int r, g, b;
    size_t prev, next;
    for (size_t i = 0; i < image->height; ++i) {
        for (size_t j = 0; j < image->width; ++j) {
            prev = i - 1;
            next = i + 1;
            if (i - 1 < 0)
                prev = i;
            else if (i + 1 > image->width)
                prev = i;
            r = abs((int) image->data[prev * image->width + j].red -
                    (int) image->data[next * image->width + j].red) / 2;
            g = abs((int) image->data[prev * image->width + j].green -
                (int) image->data[next * image->width + j].green) / 2;
            b = abs((int) image->data[prev * image->width + j].blue -
                    (int) image->data[next * image->width + j].blue) / 2;

            prev = j - 1;
            next = j + 1;
            if (j - 1 < 0)
                prev = j;
            else if (j + 1 > image->width)
                prev = j;
            r += abs((int) image->data[i * image->width + prev].red -
                    (int) image->data[i * image->width + next].red) / 2;
            g += abs((int) image->data[i * image->width + prev].green -
                    (int) image->data[i * image->width + next].green) / 2;
            b += abs((int) image->data[i * image->width + prev].blue -
                    (int) image->data[i * image->width + next].blue) / 2;

            energy[i][j] = r + g + b;
        }
    }
    return energy;
}

PNMImage *reduceImageWidth(const PNMImage *image, size_t k) {
    int **energy;
    PNMImage *reduced;
    energy = malloc(sizeof(int*) * image->height);
    for (size_t i = 0; i < image->height; ++i) {
        energy[i] = malloc(sizeof(int) * ((int) image->width));
    }
    for (size_t i = 0; i < image->width - k; ++i) {
        energy = computeEnergy(energy, image);

    }

    /*for (size_t i = 0; i < image->height; ++i) {
        for (size_t j = 0; j < image->width - k; ++j) {
            printf("%d ", energy[i][j]);
        }
        printf("\n");
    }*/

    //free matrix of energies
    for (size_t j = 0; j < image->height; ++j) {
        free(energy[j]);
    }
    free(energy);
    return reduced;
}
