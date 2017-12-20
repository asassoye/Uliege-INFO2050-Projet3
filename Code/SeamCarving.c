//
// Created by Noémie Lecocq on 20/12/17.
//

//Je fais ça du coup <3
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "SeamCarving.h"


static int **computeEnergy(int **energy, const PNMImage *image) {
    int r, g, b;
    int prev, next;
    for (int i = 0; i < (int) image->height; ++i) {
        for (int j = 0; j < (int) image->width; ++j) {
            prev = i - 1;
            next = i + 1;
            if (i - 1 < 0)
                prev = i;
            else if (i + 1 > (int) image->width)
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
            else if (j + 1 > (int) image->width)
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


static int findMin(int a, int b, int c) {
    if (a < b && a < c) {
        return a;
    } else if (b < c && b < a) {
        return b;
    } else {
        return c;
    }

}


static int **computeSum(int **energy, int height, int width) {
    //allocate sum
    int **sum = malloc(sizeof(int *) * height);
    for (int i = 0; i < height; ++i) {
        sum[i] = malloc(sizeof(int) * width);
    }

    int left, mid, right;
    for (int j = 0; j < width; ++j) {
        sum[0][j] = energy[0][j];
    }

    for (int i = 1; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (j - 1 < 0)
                left = INT_MAX;
            else
                left = sum[i - 1][j - 1];
            mid = sum[i - 1][j];
            if (j + 1 > width)
                right = INT_MAX;
            else
                right = sum[i - 1][j + 1];
            sum[i][j] = energy[i][j] + findMin(left, mid, right);
        }
    }
    return sum;
}


PNMImage *reduceImageWidth(const PNMImage *image, size_t k) {
    int **energy;
    int **sum;
    PNMImage *reduced;
    //allocate energy
    energy = malloc(sizeof(int *) * image->height);
    for (size_t i = 0; i < image->height; ++i) {
        energy[i] = malloc(sizeof(int) * ((int) image->width));
    }


    //for (size_t i = 0; i < image->width - k; ++i) {
    energy = computeEnergy(energy, image);
    sum = computeSum(energy, (int) image->height, (int) (image->width));

    //}

/*    for (size_t i = 0; i < image->height; ++i) {
        for (size_t j = 0; j < image->width - k; ++j) {
            printf("%d ", energy[i][j]);
        }
        printf("\n");
    }
    printf("\n\n\n\n");
    for (size_t i = 0; i < image->height; ++i) {
        for (size_t j = 0; j < image->width - k; ++j) {
            printf("%d ", sum[i][j]);
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
