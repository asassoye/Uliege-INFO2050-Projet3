//
// Created by Noémie Lecocq on 20/12/17.
//


//main pour faire des tests

#include <stdio.h>
#include <stdlib.h>
#include "SeamCarving.h"

int main(int argc, char *argv[]) {
    // Load image
    PNMImage *original = readPNM(argv[1]);
    if (!original) {
        fprintf(stderr, "Aborting; cannot load image '%s'\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    PNMImage *new = increaseImageWidth(original, 100);
    writePNM(argv[2], new);
    freePNM(original);
    freePNM(new);
}
