#include "homework.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int num_threads;
int resize_factor;

void readInput(const char * fileName, image *img) {
    FILE * file;

    file = fopen(fileName, "r");
    if (!file) {
        exit(-1);
    }

    fscanf(
        file,
        "P%hhu\n%d %d\n%hhu\n",
        &img -> type,
        &img -> width,
        &img -> height,
        &img -> max
    );


    if (img -> type == IMAGE_GRAYSCALE) {
        img -> pixels = (unsigned char **)malloc(img -> height * sizeof(unsigned char *));
    }
    else if (img -> type == IMAGE_COLOR) {
        img -> pixels = (unsigned char ***)malloc(img -> height * sizeof(unsigned char **));
    }
    else {
        exit(-1);
    }

    int i, j;
    for (j = 0; j < img -> height; j++) {
        if (img -> type == IMAGE_GRAYSCALE) {
            (((unsigned char**)(img -> pixels))[j]) = (unsigned char *)malloc(img -> width);
        }
        else {
            ((unsigned char***)(img -> pixels))[j] = (unsigned char **)malloc(img -> width * sizeof(unsigned char *));
        }

        for (i = 0; i < img -> width; i++) {
            if (img -> type == IMAGE_GRAYSCALE) {
                fscanf(file, "%c", &(((unsigned char**)(img -> pixels))[j][i]));
            }
            else {
                ((unsigned char***)(img -> pixels))[j][i] = (unsigned char *)malloc(3 * sizeof(unsigned char));
                fscanf(
                    file,
                    "%c%c%c",
                    &((unsigned char***)(img -> pixels))[j][i][0],
                    &((unsigned char***)(img -> pixels))[j][i][1],
                    &((unsigned char***)(img -> pixels))[j][i][2]
                );
            }
        }
    }

    fclose(file);
}

void writeData(const char * fileName, image *img) {
    FILE * file;

    file = fopen(fileName, "w");

    fprintf(
        file,
        "P%hhu\n%d %d\n%hhu\n",
        img -> type,
        img -> width,
        img -> height,
        img -> max
    );

    int i, j;
    for (j = 0; j < img -> height; j++) {
        for (i = 0; i < img -> width; i++) {
            if (img -> type == IMAGE_GRAYSCALE) {
                fprintf(
                    file,
                    "%c",
                    ((unsigned char **)(img -> pixels))[j][i]
                );
            }
            else {
                fprintf(
                    file,
                    "%c%c%c",
                    ((unsigned char ***)(img -> pixels))[j][i][0],
                    ((unsigned char ***)(img -> pixels))[j][i][1],
                    ((unsigned char ***)(img -> pixels))[j][i][2]
                );
            }
        }
    }

    fclose(file);
}

void resize(image *in, image * out) { 
    memcpy(out, in, sizeof(*in));
}