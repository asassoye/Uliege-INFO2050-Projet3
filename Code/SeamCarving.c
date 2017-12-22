//
// Created by Noémie Lecocq on 20/12/17.
//

//Je fais ça du coup <3
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>

#include "SeamCarving.h"


static void computeEnergy(int **energy, PNMImage *image) {
    int r, g, b;
    int prev, next;
    for (int i = 0; i < (int) image->height; ++i) {
        for (int j = 0; j < (int) image->width; ++j) {
            prev = i - 1;
            next = i + 1;
            if (i - 1 < 0)
                prev = i;
            else if (i + 1 >= (int) image->height)
                next = i;
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
            else if (j + 1 >= (int) image->width)
                next = j;
            r += abs((int) image->data[i * image->width + prev].red -
                     (int) image->data[i * image->width + next].red) / 2;
            g += abs((int) image->data[i * image->width + prev].green -
                     (int) image->data[i * image->width + next].green) / 2;
            b += abs((int) image->data[i * image->width + prev].blue -
                     (int) image->data[i * image->width + next].blue) / 2;

            energy[i][j] = r + g + b;
        }
    }
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


static void computeSum(int **energy, int **sum, int height, int width) {
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
            if (j + 1 >= width)
                right = INT_MAX;
            else
                right = sum[i - 1][j + 1];
            sum[i][j] = energy[i][j] + findMin(left, mid, right);
        }
    }
}

//save the positions
static int *findSeam(int **sum, int *seam, int height, int width) {
    int min = INT_MAX;
    int dest = 0;
    for (int i = 0; i < width; ++i) {
        if (sum[height - 1][i] < min) {
            min = sum[height - 1][i];
            dest = i;
        }
    }
    seam[height - 1] = dest;
    for (int i = height - 2; i >= 0; --i) {
        if (dest - 1 < 0) {
            if (sum[i][dest] < sum[i][dest + 1]) {
                seam[i] = dest;
            } else {
                seam[i] = dest + 1;
            }
        } else if (dest + 1 >= height) {
            if (sum[i][dest - 1] < sum[i][dest]) {
                seam[i] = dest - 1;
            }
        } else {
            if (sum[i][dest - 1] < sum[i][dest] && sum[i][dest - 1] < sum[i][dest + 1]) {
                seam[i] = dest - 1;
            } else if (sum[i][dest] < sum[i][dest + 1] && sum[i][dest] < sum[i][dest - 1]) {
                seam[i] = dest;
            } else {
                seam[i] = dest + 1;
            }
        }
        dest = seam[i];
    }
    return seam;
}

static PNMImage *removeSeam(PNMImage *new, PNMImage *image, const int *seam) {
    new->width--;

    int index;
    for (int i = 0; i < image->height; ++i) {
        index = 0;
        for (int j = 0; j < image->width; ++j) {
            if (seam[i] != j) {
                new->data[i * new->width + index].red = image->data[i * image->width + j].red;
                new->data[i * new->width + index].green = image->data[i * image->width + j].green;
                new->data[i * new->width + index].blue = image->data[i * image->width + j].blue;
                index++;
            }
        }
    }
    return new;
}

static void PNMcpy(PNMImage *dest, PNMImage *src) {
    dest->width = src->width;
    dest->height = src->height;
    for (int i = 0; i < src->height * src->width; ++i) {
        dest->data[i] = src->data[i];
    }
}


PNMImage *reduceImageWidth(const PNMImage *image, size_t k) {
    int **energy;
    int **sum;
    int *seam;
    PNMImage *reduced = createPNM(image->width, image->height);
    PNMImage *tmp = createPNM(image->width, image->height);
    //allocate energy
    energy = malloc(sizeof(int *) * image->height);
    for (size_t i = 0; i < image->height; ++i) {
        energy[i] = malloc(sizeof(int) * ((int) image->width));
    }
    //allocate sum
    sum = malloc(sizeof(int *) * image->height);
    for (int i = 0; i < image->height; ++i) {
        sum[i] = malloc(sizeof(int) * image->width);
    }
    //allocate seam
    seam = malloc(sizeof(int) * image->height);
    PNMcpy(tmp, (PNMImage *) image);
    for (size_t i = 0; i < image->width - k; ++i) {
        computeEnergy(energy, tmp);
        computeSum(energy, sum, (int) (image->height), (int) (image->width - i));
        findSeam(sum, seam, (int) (image->height), (int) (image->width - i));
        reduced = removeSeam(reduced, tmp, seam);
        PNMcpy(tmp, reduced);
    }


    //free matrix of energies
    for (size_t j = 0; j < image->height; ++j) {
        free(energy[j]);
    }
    free(energy);
    for (size_t j = 0; j < image->height; ++j) {
        free(sum[j]);
    }
    free(sum);
    free(seam);
    freePNM(reduced);

    return tmp;
}
