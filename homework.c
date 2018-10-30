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

    if (img -> type == IMAGE_GRAYSCALE) {
        for (j = 0; j < img -> height; j++) {
            (((unsigned char**)(img -> pixels))[j]) = (unsigned char *)malloc(img -> width);
        
            fread((((unsigned char**)(img -> pixels))[j]), 1, img -> width, file);
        }
    }
    else {
        for (j = 0; j < img -> height; j++) {
            ((unsigned char***)(img -> pixels))[j] = (unsigned char **)malloc(img -> width * sizeof(unsigned char *));

            for (i = 0; i < img -> width; i++) {
                ((unsigned char***)(img -> pixels))[j][i] = (unsigned char *)malloc(3 * sizeof(unsigned char));
                fread(((unsigned char***)(img -> pixels))[j][i], 1, 3, file);
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
    if (img -> type == IMAGE_GRAYSCALE) {
        for (j = 0; j < img -> height; j++) {
            fwrite(((unsigned char **)(img -> pixels))[j], 1, img -> width, file);
        }
    }
    else {
        for (j = 0; j < img -> height; j++) {
            for (i = 0; i < img -> width; i++) {
                fwrite(((unsigned char ***)(img -> pixels))[j][i], 1, 3, file);
            }
        }
    }

    fclose(file);
}

void * threadFunction (void * var) {
    options opt = *(options*)var;

    image * in = opt.in;
    image * out = opt.out;

    int i, j, a, b;

    int rf_2 = resize_factor * resize_factor;
    int sumR, sumG, sumB;
    int jrf, irf, jrf1, jrf2, jrfa, irf1, irf2, irfb;

    for (j = opt.start; j < opt.stop; j++) {
        if (out -> type == IMAGE_GRAYSCALE) {
            (((unsigned char**)(out -> pixels))[j]) = (unsigned char *)malloc(out -> width);
        }
        else {
            ((unsigned char***)(out -> pixels))[j] = (unsigned char **)malloc(out -> width * sizeof(unsigned char *));
        }

        jrf = j * resize_factor;
        jrf1 = jrf + 1;
        jrf2 = jrf + 2;

        if (out -> type == IMAGE_GRAYSCALE && resize_factor != 3) {
            for (i = 0; i < out -> width; i++) {
                sumR = 0;
                irf = i * resize_factor;

                for (a = 0; a < resize_factor; a++) {
                    jrfa = jrf + a;

                    for (b = 0; b < resize_factor; b++) {
                        irfb = irf + b;

                        sumR += ((unsigned char**)(in -> pixels))[jrfa][irfb];
                    }
                }

                ((unsigned char**)(out -> pixels))[j][i] = sumR / rf_2;
            }
        }
        else if (out -> type == IMAGE_GRAYSCALE && resize_factor == 3) {
            for (i = 0; i < out -> width; i++) {
                irf = i * resize_factor;
                irf1 = irf + 1;
                irf2 = irf + 2;

                ((unsigned char**)(out -> pixels))[j][i] = (
                    ((unsigned char**)(in -> pixels))[jrf][irf]
                    + 2 * ((unsigned char**)(in -> pixels))[jrf][irf1]
                    + ((unsigned char**)(in -> pixels))[jrf][irf2]
                    + 2 * ((unsigned char**)(in -> pixels))[jrf1][irf]
                    + 4 * ((unsigned char**)(in -> pixels))[jrf1][irf1]
                    + 2 * ((unsigned char**)(in -> pixels))[jrf1][irf2]
                    + ((unsigned char**)(in -> pixels))[jrf2][irf]
                    + 2 * ((unsigned char**)(in -> pixels))[jrf2][irf1]
                    + ((unsigned char**)(in -> pixels))[jrf2][irf2]
                ) / 16;
            }
        }
        else if (out -> type == IMAGE_COLOR && resize_factor != 3) {
            for (i = 0; i < out -> width; i++) {
                ((unsigned char***)(out -> pixels))[j][i] = (unsigned char *)malloc(3 * sizeof(unsigned char));
                
                irf = i * resize_factor;
                sumR = sumG = sumB = 0;

                for (a = 0; a < resize_factor; a++) {
                    jrfa = jrf + a;

                    for (b = 0; b < resize_factor; b++) {
                        irfb = irf + b;

                        sumR += ((unsigned char***)(in -> pixels))[jrfa][irfb][0];
                        sumG += ((unsigned char***)(in -> pixels))[jrfa][irfb][1];
                        sumB += ((unsigned char***)(in -> pixels))[jrfa][irfb][2];
                    }
                }

                ((unsigned char***)(out -> pixels))[j][i][0] = sumR / rf_2;
                ((unsigned char***)(out -> pixels))[j][i][1] = sumG / rf_2;
                ((unsigned char***)(out -> pixels))[j][i][2] = sumB / rf_2;
            }
        }
        else if (out -> type == IMAGE_COLOR && resize_factor == 3) {
            for (i = 0; i < out -> width; i++) {
                ((unsigned char***)(out -> pixels))[j][i] = (unsigned char *)malloc(3 * sizeof(unsigned char));

                irf = i * resize_factor;
                irf1 = irf + 1;
                irf2 = irf + 2;

                ((unsigned char***)(out -> pixels))[j][i][0] = (
                    ((unsigned char***)(in -> pixels))[jrf][irf][0]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf][irf1][0]
                    + ((unsigned char***)(in -> pixels))[jrf][irf2][0]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf1][irf][0]
                    + 4 * ((unsigned char***)(in -> pixels))[jrf1][irf1][0]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf1][irf2][0]
                    + ((unsigned char***)(in -> pixels))[jrf2][irf][0]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf2][irf1][0]
                    + ((unsigned char***)(in -> pixels))[jrf2][irf2][0]
                ) / 16;
                ((unsigned char***)(out -> pixels))[j][i][1] = (
                    ((unsigned char***)(in -> pixels))[jrf][irf][1]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf][irf1][1]
                    + ((unsigned char***)(in -> pixels))[jrf][irf2][1]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf1][irf][1]
                    + 4 * ((unsigned char***)(in -> pixels))[jrf1][irf1][1]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf1][irf2][1]
                    + ((unsigned char***)(in -> pixels))[jrf2][irf][1]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf2][irf1][1]
                    + ((unsigned char***)(in -> pixels))[jrf2][irf2][1]
                ) / 16;
                ((unsigned char***)(out -> pixels))[j][i][2] = (
                    ((unsigned char***)(in -> pixels))[jrf][irf][2]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf][irf1][2]
                    + ((unsigned char***)(in -> pixels))[jrf][irf2][2]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf1][irf][2]
                    + 4 * ((unsigned char***)(in -> pixels))[jrf1][irf1][2]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf1][irf2][2]
                    + ((unsigned char***)(in -> pixels))[jrf2][irf][2]
                    + 2 * ((unsigned char***)(in -> pixels))[jrf2][irf1][2]
                    + ((unsigned char***)(in -> pixels))[jrf2][irf2][2]
                ) / 16;
            }
        }
        else {
            return NULL;
        }
    }

    return NULL;
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