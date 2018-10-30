#include "homework.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


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

void * threadFunction (void * var) {
    options opt = *(options*)var;

    image * in = opt.in;
    image * out = opt.out;

    int i, j, a, b, c;

    int rf_2 = resize_factor * resize_factor;
    int sumR, sumG, sumB;
    int jrf, irf;

    for (j = opt.start; j < opt.stop; j++) {
        if (out -> type == IMAGE_GRAYSCALE) {
            (((unsigned char**)(out -> pixels))[j]) = (unsigned char *)malloc(out -> width);
        }
        else {
            ((unsigned char***)(out -> pixels))[j] = (unsigned char **)malloc(out -> width * sizeof(unsigned char *));
        }

        for (i = 0; i < out -> width; i++) {
            sumR = sumG = sumB = 0;
            jrf = j * resize_factor;
            irf = i * resize_factor;

            // I'm going to hell for this
            // Eram prea obosit
            if (out -> type == IMAGE_GRAYSCALE) {
                if (resize_factor != 3) {
                    for (a = 0; a < resize_factor; a++) {
                        for (b = 0; b < resize_factor; b++) {
                            sumR += ((unsigned char**)(in -> pixels))[jrf + a][irf + b];
                        }
                    }

                    ((unsigned char**)(out -> pixels))[j][i] = sumR / rf_2;
                }
                else {
                    ((unsigned char**)(out -> pixels))[j][i] = (
                        ((unsigned char**)(in -> pixels))[jrf + 0][irf + 0]
                        + 2 * ((unsigned char**)(in -> pixels))[jrf + 0][irf + 1]
                        + ((unsigned char**)(in -> pixels))[jrf + 0][irf + 2]
                        + 2 * ((unsigned char**)(in -> pixels))[jrf + 1][irf + 0]
                        + 4 * ((unsigned char**)(in -> pixels))[jrf + 1][irf + 1]
                        + 2 * ((unsigned char**)(in -> pixels))[jrf + 1][irf + 2]
                        + ((unsigned char**)(in -> pixels))[jrf + 2][irf + 0]
                        + 2 * ((unsigned char**)(in -> pixels))[jrf + 2][irf + 1]
                        + ((unsigned char**)(in -> pixels))[jrf + 2][irf + 2]
                    ) / 16;
                }
            }
            else {
                ((unsigned char***)(out -> pixels))[j][i] = (unsigned char *)malloc(3 * sizeof(unsigned char));
                
                if (resize_factor != 3) {
                    for (a = 0; a < resize_factor; a++) {
                        for (b = 0; b < resize_factor; b++) {
                            sumR += ((unsigned char***)(in -> pixels))[jrf + a][irf + b][0];
                            sumG += ((unsigned char***)(in -> pixels))[jrf + a][irf + b][1];
                            sumB += ((unsigned char***)(in -> pixels))[jrf + a][irf + b][2];
                        }
                    }

                    ((unsigned char***)(out -> pixels))[j][i][0] = sumR / rf_2;
                    ((unsigned char***)(out -> pixels))[j][i][1] = sumG / rf_2;
                    ((unsigned char***)(out -> pixels))[j][i][2] = sumB / rf_2;
                }
                else {
                    c = 0;
                    ((unsigned char***)(out -> pixels))[j][i][c] = (
                        ((unsigned char***)(in -> pixels))[jrf + 0][irf + 0][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 0][irf + 1][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 0][irf + 2][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 0][c]
                        + 4 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 1][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 2][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 2][irf + 0][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 2][irf + 1][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 2][irf + 2][c]
                    ) / 16;
                    c = 1;
                    ((unsigned char***)(out -> pixels))[j][i][c] = (
                        ((unsigned char***)(in -> pixels))[jrf + 0][irf + 0][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 0][irf + 1][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 0][irf + 2][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 0][c]
                        + 4 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 1][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 2][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 2][irf + 0][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 2][irf + 1][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 2][irf + 2][c]
                    ) / 16;
                    c = 2;
                    ((unsigned char***)(out -> pixels))[j][i][c] = (
                        ((unsigned char***)(in -> pixels))[jrf + 0][irf + 0][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 0][irf + 1][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 0][irf + 2][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 0][c]
                        + 4 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 1][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 1][irf + 2][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 2][irf + 0][c]
                        + 2 * ((unsigned char***)(in -> pixels))[jrf + 2][irf + 1][c]
                        + ((unsigned char***)(in -> pixels))[jrf + 2][irf + 2][c]
                    ) / 16;
                }


            }
        }
    }
}

void resize(image *in, image * out) { 
    out -> max = in -> max;
    out -> type = in -> type;
    out -> width = in -> width / resize_factor;
    out -> height = in -> height / resize_factor;

    pthread_t tid[num_threads];
    options opt[num_threads];

    if (out -> type == IMAGE_GRAYSCALE) {
        out -> pixels = (unsigned char **)malloc(out -> height * sizeof(unsigned char *));
    }
    else if (out -> type == IMAGE_COLOR) {
        out -> pixels = (unsigned char ***)malloc(out -> height * sizeof(unsigned char **));
    }
    else {
        exit(-1);
    }

    int i;

    for (i = 0; i < num_threads; i++) {
        opt[i].in = in;
        opt[i].out = out;
        opt[i].start = (out -> height / num_threads) * i;
        opt[i].stop = (out -> height / num_threads) * (i+1);

        if (i == num_threads - 1) {
            opt[i].stop = out -> height;
        }
    }

    for (i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(opt[i]));
    }

    for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}