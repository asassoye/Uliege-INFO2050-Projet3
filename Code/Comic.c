
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "Comic.h"
#include "PNM.h"
#include "SeamCarving.h"

#define BG_RED 255;
#define BG_GREEN 255;
#define BG_BLUE 255;


static unsigned long cost(const PNMImage **images, size_t comicWidth, size_t comicBorder, size_t i, size_t j);

static long extras(const PNMImage **images, size_t comicWidth, size_t comicBorder, size_t i, size_t j);

static unsigned long *optimal(long unsigned **costMatrix, size_t *parents, size_t j, size_t nbImages);

static int composePlacement(size_t *parent, size_t indice, size_t *placement);

static unsigned long **createEmptyMatrix(size_t i, size_t j);

static void freeMatrix(void **matrix, size_t size);

static long min(long a, long b);

static void copierImage(PNMImage *conteneur, const PNMImage *image, size_t x, size_t y);


size_t *wrapImages(const PNMImage **images, size_t nbImages, size_t comicWidth, size_t comicBorder) {
    /*
     * Précondition
     */
    assert(images != NULL);
    assert(nbImages != 0);
    assert(comicWidth != 0);
    assert(comicBorder != 0);

    /*
     * On crée le vecteur de placement des images par rangée.
     */
    size_t *placement = calloc(nbImages, sizeof(size_t));

    /*
     * Si l'allocation ne fonctionne pas
     */
    if (placement == NULL) {
        fprintf(stderr, "Aborting; unable to allocate memory\n");
        return NULL;
    }

    /*
     * On crée la matrice qui contiendra toutes les valeurs de cost.
     */
    long unsigned int **costMatrix;
    costMatrix = createEmptyMatrix(nbImages, nbImages);


    size_t mem[nbImages + 1];

    /*
     * On calcule toutes les valeurs de cost
     */
    for (size_t i = 0; i < nbImages; i++) {
        for (size_t j = i; j < nbImages; j++) {
            costMatrix[i][j] = cost(images, comicWidth, comicBorder, i + 1, j + 1);
        }
    }

    /*
     * On rempli optimalCost
     */
    optimal(costMatrix, mem, 0, nbImages);


    /*
     * On crée le tableau de l'emplacement de chaque image a partir du tableau mem
     */

    composePlacement(mem, nbImages, placement);


    /*
     * On libère l'espace alloué.
     */
    freeMatrix((void **) costMatrix, nbImages);

    return placement;

}

PNMImage *packComic(const PNMImage **images, size_t nbImages, size_t comicWidth,
                    size_t comicBorder) {

    size_t *wrap = wrapImages(images, nbImages, comicWidth, comicBorder);

    fprintf(stderr, "Comic width: %zu\n", comicWidth);
    fprintf(stderr, "Comic boder: %zu\n", comicBorder);
    fprintf(stderr, "NB: %zu\n", nbImages);
    fprintf(stderr, "Table: ");

    for (unsigned j = 0; j < nbImages; ++j) {
        fprintf(stderr, "%zu ", wrap[j]);
    }
    fprintf(stderr, "\n");

    /*
     * Si le calcul de la justification echoue.
     */
    if (wrap == NULL) {
        free(wrap);
        return NULL;
    }

    size_t lignes = wrap[nbImages - 1] + 1;


    PNMImage *result = createPNM(comicWidth, (lignes * (images[0]->height + comicBorder) + comicBorder));

    /*
     * Si l'allocation echoue.
     */
    if (result == NULL) {
        return NULL;
    }

    /*
     * Arriere plan
     */
    for (unsigned i = 0; i < result->height; ++i) {
        for (unsigned j = 0; j < result->width; ++j) {
            result->data[result->width * i + j].red = BG_RED;
            result->data[result->width * i + j].green = BG_GREEN;
            result->data[result->width * i + j].blue = BG_BLUE;
        }
    }

    size_t *imageParLigne = calloc(lignes, sizeof(size_t));
    unsigned long tmp = 0, cmp = 0;

    for (size_t i = 0; i < nbImages; ++i) {
        if (wrap[i] == tmp) {
            cmp++;
        } else {
            imageParLigne[tmp] = (size_t) cmp;
            cmp = 1;
            tmp++;
        }
    }

    PNMImage **redimension = malloc(nbImages * sizeof(*redimension));

    size_t actualWidth = comicBorder;
    int diference = 0;

    cmp = 0;
    for (unsigned k = 0; k < lignes; ++k) {
        for (unsigned i = 0; i < imageParLigne[k]; ++i) {
            actualWidth += images[cmp]->width + comicBorder;
            cmp++;
        }
        diference = (((int) comicWidth) - ((int) actualWidth)) / (int) imageParLigne[k];

        if (diference < 0) {
            cmp -= imageParLigne[k];
            for (unsigned i = 0; i < imageParLigne[k]; ++i) {
                redimension[cmp] = reduceImageWidth(images[cmp], (size_t) abs(diference));
                cmp++;
            }
        } else if (diference > 0) {
            cmp -= imageParLigne[k];
            for (unsigned i = 0; i < imageParLigne[k]; ++i) {
                redimension[cmp] = increaseImageWidth(images[cmp], (size_t) abs(diference));
                cmp++;
            }
        }
    }


    cmp = 0;
    for (unsigned k = 0; k < lignes; ++k) {
        tmp = comicBorder;
        for (unsigned i = 0; i < imageParLigne[k]; ++i) {
            copierImage(result, redimension[cmp], comicBorder + tmp,
                        comicBorder + k * (images[0]->height + comicBorder));
            tmp += images[cmp]->width + comicBorder;
            cmp++;
        }
    }

    free(redimension);
    free(imageParLigne);
    free(wrap);

    return result;

}

static void copierImage(PNMImage *conteneur, const PNMImage *image, size_t x, size_t y) {
    size_t posX, posY;

    for (size_t i = 0; i < image->height; ++i) {
        for (unsigned j = 0; j < image->width; ++j) {
            posX = j + x;
            posY = i + y;

            /*
             * On copie le pixel que si il est dans les limites de l'image
             */
            if (posX < conteneur->width && posY < conteneur->height)
                conteneur->data[posY * conteneur->width + posX] = image->data[i * image->width + j];
        }
    }
}

static void freeMatrix(void **matrix, size_t size) {
    /*
     * Précondition
     */
    assert(matrix != NULL);

    for (size_t k = 0; k < size; ++k) {
        if (matrix[k] != NULL) {
            free(matrix[k]);
        }

    }

    free(matrix);
}

static unsigned long cost(const PNMImage **images, size_t comicWidth, size_t comicBorder, size_t i, size_t j) {

    return (unsigned long) pow(labs(extras(images, comicWidth, comicBorder, i, j)), 3);

}

static long extras(const PNMImage **images, size_t comicWidth, size_t comicBorder, size_t i, size_t j) {
    unsigned long total = 0;

    for (size_t k = i - 1; k < j; k++) {
        total += images[k]->width;

        /*
         * Si on est pas a la derniere, ajoute la bordure.
         */
        if (k != j - 1) {
            total += comicBorder;
        }

    }

    return comicWidth - total;
}

static unsigned long *optimal(long unsigned **costMatrix, size_t *parents, size_t j, size_t nbImages) {

    unsigned long *optimalCost = calloc(nbImages + 1, sizeof(unsigned long));

    if (optimalCost == NULL) {
        fprintf(stderr, "Aborting; unable to allocate memory \n");
        exit(EXIT_FAILURE);
    }

    if (j <= nbImages) {
        if (j == 0) {
            optimalCost[j] = 0;
        } else {
            optimalCost[j] = ULONG_MAX;

            for (size_t i = 1; i <= j; ++i) {
                if ((optimalCost[i - 1] + costMatrix[i - 1][j - 1] < optimalCost[j])) {
                    optimalCost[j] = optimalCost[i - 1] + costMatrix[i - 1][j - 1];
                    parents[j] = i;
                }
            }
        }

        optimal(costMatrix, parents, j + 1, nbImages);
    }

    free(optimalCost);

    return optimalCost;
}

static long min(long a, long b) {
    return a > b ? b : a;
}

static int composePlacement(size_t *parent, size_t indice, size_t *placement) {
    int current;
    if (parent[indice] == 1) {
        current = 1;
    } else {
        current = 1 + composePlacement(parent, parent[indice] - 1, placement);
    }

    for (size_t i = parent[indice] - 1; i < indice; ++i) {
        placement[i] = (size_t) (current - 1);
    }

    return current;
}

static unsigned long **createEmptyMatrix(size_t i, size_t j) {
    /*
     * On crée les lignes
     */
    unsigned long int **matrix = calloc(i, sizeof(void *));

    /*
     * Si l'allocation ne marche pas,
     */
    if (matrix == NULL) {
        fprintf(stderr, "Aborting; unable to allocate memory\n");
        return NULL;
    }

    /*
     * On crée les colonnes.
     */
    for (size_t k = 0; k < j; k++) {
        matrix[k] = calloc(j, sizeof(long unsigned int));

        /*
         * Si l'allocation échoue.
         */
        if (matrix[k] == NULL) {
            fprintf(stderr, "Aborting; unable to allocate memory \n");
            freeMatrix((void **) matrix, j);
            exit(EXIT_FAILURE);
        }
    }

    return matrix;

}


