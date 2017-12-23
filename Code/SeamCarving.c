/*
 * Noémie Lecocq s130165
 * Andrew Sassoye
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <assert.h>

#include "SeamCarving.h"

/** Copie scr dans dest
 *
 * @param dest PNMImage de destination
 * @param src PNMImage à copier
 */
static void PNMcpy(PNMImage *dest, PNMImage *src);

/** Calcule l'énergie de chaque pixel et enregistre le résultat dans le tableau energy
 *
 * @param energy Tableau à 2 dimensions de la même taille que l'image
 * @param image L'image dont on doit calculer l'énergie
 */
static void computeEnergy(int **energy, PNMImage *image);

/** Trouve le minimum de 3 nombres
 *
 * @param a Nombre 1
 * @param b Nombre 2
 * @param c Nombre 3
 * @return Le minimum
 */
static int findMin(int a, int b, int c);

/** Calcule le cout minimal du chemin vers chaque pixel
 *
 * @param energy Tableau contenant l'énergie de chaque pixel
 * @param sum Tableau de même taille que energy
 * @param height Hauteur du tableau
 * @param width Largeur du tableau
 */
static void computeCost(int **energy, int **sum, int height, int width);

/** Calcule récursivement le cout du chemin vers le pixel (i,j)
 *
 * @param energy Tableau contenant l'énergie de chaque pixel
 * @param sum Tableau contenant les couts
 * @param width Largeur du tableau
 * @param i Coordonnée du pixel
 * @param j Coordonnée du pixel
 */
static void recursiveCost(int **energy, int **sum, int width, int i, int j);

/** Trouve la couture d'énergie minimale
 *
 * @param sum Tableau contenant les couts
 * @param seam Chemin du haut vers le bas de l'image
 * @param height Hauteur du tableau
 * @param width Largeur du tableau
 */
static void findSeam(int **sum, int *seam, int height, int width);

/** Retire la couture d'énergie minimale
 *
 * @param new L'image résultante
 * @param image L'image originale
 * @param seam La couture d'énergie minimale
 */
static void removeSeam(PNMImage *new, PNMImage *image, const int *seam);

/** Marque les pixels à retirer
 *
 * @param seam La couture d'énergie minimale
 * @param seams Tableau contenant les pixels déjà retirés
 * @param marked Tableau de pixels
 * @param height Hauteur du tableau
 * @param width Largeur du tableau
 * @param k La position dans le tableau seams
 */
static void markSeam(const int *seam, int **seams, int **marked, int height, int width, size_t k);

/** Augmente la taille de l'image en ajoutant un pixel à droite de chaque pixel marqué
 *
 * @param image L'image originale
 * @param marked Tableau de pixels marqués
 * @param k Le nombre de pixel à ajouter en largeur
 * @return L'image résultante
 */
static PNMImage *increasePNM(PNMImage *image, int **marked, size_t k);

/** Augmente la taille de l'image de k pixel en utilisant le seam carving
 *
 * @param image L'image originale
 * @param k Le nombre de pixel à ajouter en largeur
 * @return L'image résultante
 */
static PNMImage *increaseImage(const PNMImage *image, size_t k);


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
    for (size_t i = 0; i < image->height; ++i) {
        sum[i] = malloc(sizeof(int) * image->width);
    }
    //allocate seam
    seam = malloc(sizeof(int) * image->height);

    PNMcpy(tmp, (PNMImage *) image);
    for (size_t i = 0; i < k; ++i) {
        computeEnergy(energy, tmp);
        computeCost(energy, sum, (int) tmp->height, (int) tmp->width);
        findSeam(sum, seam, (int) (image->height), (int) (image->width - i));
        removeSeam(reduced, tmp, seam);
        PNMcpy(tmp, reduced);
    }
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

PNMImage *increaseImageWidth(const PNMImage *image, size_t k) {
    PNMImage *result = createPNM(image->width, image->height);
    PNMcpy(result, (PNMImage *) image);
    PNMImage *tmp;
    while (k > result->width * 20 / 100) {
        tmp = increaseImage(result, result->width * 20 / 100);
        k -= image->width * 20 / 100;
        result = tmp;
    }
    tmp = increaseImage(result, k);
    freePNM(result);
    return tmp;
}

static PNMImage *increaseImage(const PNMImage *image, size_t k) {
    int **energy;
    int **sum;
    int *seam;
    int **marked;
    int **seams;
    PNMImage *reduced = createPNM(image->width, image->height);
    PNMImage *tmp = createPNM(image->width, image->height);

    //allocate energy
    energy = malloc(sizeof(int *) * image->height);
    for (size_t i = 0; i < image->height; ++i) {
        energy[i] = malloc(sizeof(int) * image->width);
    }
    //allocate sum
    sum = malloc(sizeof(int *) * image->height);
    for (size_t i = 0; i < image->height; ++i) {
        sum[i] = malloc(sizeof(int) * image->width);
    }
    //allocate seam
    seam = malloc(sizeof(int) * image->height);

    //allocate seams
    seams = malloc(sizeof(int *) * image->height);
    for (size_t i = 0; i < image->height; ++i) {
        seams[i] = malloc(sizeof(int) * k);
    }

    //allocate marked
    marked = malloc(sizeof(int *) * image->height);
    for (size_t i = 0; i < image->height; ++i) {
        marked[i] = calloc(image->width, sizeof(int));
    }

    PNMcpy(tmp, (PNMImage *) image);
    for (size_t i = 0; i < k; ++i) {
        computeEnergy(energy, tmp);
        computeCost(energy, sum, (int) tmp->height, (int) tmp->width);
        findSeam(sum, seam, (int) (image->height), (int) tmp->width);
        markSeam(seam, seams, marked, (int) image->height, (int) image->width, i);
        removeSeam(reduced, tmp, seam);
        PNMcpy(tmp, reduced);
    }


    PNMImage *increased = increasePNM((PNMImage *) image, marked, k);

    //free matrix of energies
    for (size_t j = 0; j < image->height; ++j) {
        free(energy[j]);
    }
    free(energy);

    for (size_t j = 0; j < image->height; ++j) {
        free(sum[j]);
    }
    free(sum);

    //free marked
    for (size_t i = 0; i < image->height; ++i) {
        free(marked[i]);
    }
    free(marked);

    for (size_t i = 0; i < image->height; ++i) {
        free(seams[i]);
    }
    free(seams);

    free(seam);
    freePNM(reduced);
    freePNM(tmp);

    return increased;
}

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

static void recursiveCost(int **energy, int **sum, int width, int i, int j) {
    int left, mid, right;
    if (i == 0 && sum[i][j] == INT_MAX) {
        sum[i][j] = energy[i][j];
        return;
    }
    if (j - 1 >= 0 && sum[i - 1][j - 1] == INT_MAX) {
        recursiveCost(energy, sum, width, i - 1, j - 1);
    }
    if (sum[i - 1][j] == INT_MAX)
        recursiveCost(energy, sum, width, i - 1, j);
    if (j + 1 < width && sum[i - 1][j + 1] == INT_MAX) {
        recursiveCost(energy, sum, width, i - 1, j + 1);
    }
    if (j - 1 < 0)
        left = INT_MAX;
    else
        left = sum[i - 1][j - 1];
    mid = sum[i - 1][j];
    if (j + 1 >= width)
        right = INT_MAX;
    else
        right = sum[i - 1][j + 1];

    sum[i][j] = energy[i][j] + findMin(left, right, mid);
}

static void computeCost(int **energy, int **sum, int height, int width) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            sum[i][j] = INT_MAX;
        }
    }
    for (int j = 0; j < width; ++j) {
        recursiveCost(energy, sum, width, height - 1, j);
    }
}

static void findSeam(int **sum, int *seam, int height, int width) {
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
        } else if (dest + 1 > width - 1) {
            if (sum[i][dest - 1] < sum[i][dest]) {
                seam[i] = dest - 1;
            } else {
                seam[i] = dest;
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
}

static void removeSeam(PNMImage *new, PNMImage *image, const int *seam) {
    new->width--;

    int index;
    for (size_t i = 0; i < image->height; ++i) {
        index = 0;
        for (size_t j = 0; j < image->width; ++j) {
            if (seam[i] != (int) j) {
                new->data[i * new->width + index].red = image->data[i * image->width + j].red;
                new->data[i * new->width + index].green = image->data[i * image->width + j].green;
                new->data[i * new->width + index].blue = image->data[i * image->width + j].blue;
                index++;
            }
        }
    }
}

static void PNMcpy(PNMImage *dest, PNMImage *src) {
    assert(dest != NULL && src != NULL);

    dest->width = src->width;
    dest->height = src->height;
    for (size_t i = 0; i < src->height * src->width; ++i) {
        dest->data[i] = src->data[i];
    }
}

static void markSeam(const int *seam, int **seams, int **marked, int height, int width, size_t k) {
    int index = 0;
    for (int i = 0; i < height; ++i) {
        seams[i][k] = seam[i];
    }
    for (int i = 0; i < height; ++i) {
        index = 0;
        for (size_t j = 0; j < k; ++j) {
            if (seams[i][j] <= seam[i]) {
                index++;
            }
        }
        if (seam[i] + index > width - 1) {
            while (marked[i][seam[i] + index] == 1)
                index--;
        } else {
            while (marked[i][seam[i] + index] == 1) {
                index++;
                if (seam[i] + index > width - 1) {
                    while (marked[i][seam[i] + index] == 1)
                        index--;
                    break;
                }
            }
        }
        marked[i][seam[i] + index] = 1;
    }
}

static PNMImage *increasePNM(PNMImage *image, int **marked, size_t k) {
    PNMImage *new = createPNM(image->width + k, image->height);
    int index = 0;
    PNMPixel rightPixel;
    for (size_t i = 0; i < image->height; ++i) {
        index = 0;
        for (size_t j = 0; j < image->width; ++j) {
            new->data[i * new->width + index].red = image->data[i * image->width + j].red;
            new->data[i * new->width + index].green = image->data[i * image->width + j].green;
            new->data[i * new->width + index].blue = image->data[i * image->width + j].blue;
            index++;

            //double the pixel if necessary
            if (marked[i][j] == 1) {
                if (j + 1 > image->height - 1)
                    rightPixel = image->data[i * image->width + j];
                else
                    rightPixel = image->data[i * image->width + j + 1];
                new->data[i * new->width + index].red = (unsigned char)
                        (((int) (image->data[i * image->width + j].red) +
                          (int) (rightPixel.red)) / 2);
                new->data[i * new->width + index].green = (unsigned char)
                        (((int) (image->data[i * image->width + j].green) +
                          (int) (rightPixel.green)) / 2);
                new->data[i * new->width + index].blue = (unsigned char)
                        (((int) (image->data[i * image->width + j].blue) +
                          (int) (rightPixel.blue)) / 2);
                index++;
            }
        }
    }
    return new;
}
