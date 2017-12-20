//
// Created by Noémie Lecocq on 20/12/17.
//

//Je fais ça du coup <3
#include <stdio.h>
#include <stdlib.h>

#include "PNM.h"

int **computeEnergy(int **energy, const PNMImage *image) {
    int r, g, b;
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            r = abs((int) image->data[(i - 1) * image->width + j].red -
                    (int) image->data[(i + 1) * image->width + j].red) / 2;
            g = abs((int) image->data[(i - 1) * image->width + j].green -
                (int) image->data[(i + 1) * image->width + j].green) / 2;
            b = abs((int) image->data[(i - 1) * image->width + j].blue -
                    (int) image->data[(i + 1) * image->width + j].blue) / 2;
            energy[i][j] = r + g + b;
        }
    }
    return energy;
}

PNMImage *reduceImageWidth(const PNMImage *image, size_t k) {
    int **energy;
    PNMImage *reduced;

    for (int i = 0; i < k - image->width; ++i) {
        energy = malloc(sizeof(int*) * image->height);
        for (int j = 0; j < image->height; ++j) {
            energy[j] = malloc(sizeof(int) * image->width - i);
        }
        energy = computeEnergy(energy, image);
    }
    return reduced;
}
