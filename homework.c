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


    img -> pixels = (unsigned char **)malloc(img -> height * sizeof(unsigned char *));

    int j;

    int width = img -> width;
    if (img -> type == IMAGE_COLOR) {
        width *= 3;
    }

    for (j = 0; j < img -> height; j++) {
        (((unsigned char**)(img -> pixels))[j]) = (unsigned char *)malloc(width);
    
        fread((((unsigned char**)(img -> pixels))[j]), 1, width, file);
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

    int j;
    int width = img -> width;
    if (img -> type == IMAGE_COLOR) {
        width *= 3;
    }

    for (j = 0; j < img -> height; j++) {
        fwrite(((unsigned char **)(img -> pixels))[j], 1, width, file);
    }

    fclose(file);
}

void * threadFunction (void * var) {
    options opt = *(options*)var;

    image * in = opt.in;
    image * out = opt.out;

    int i, j, a, b, i0, i1, i2;

    int rf_2 = resize_factor * resize_factor;
    int sumR, sumG, sumB;
    int jrf, irf, jrf1, jrf2, jrfa, irf1, irf2, irfb, irfb1, irfb2;

    for (j = opt.start; j < opt.stop; j++) {
        if (out -> type == IMAGE_GRAYSCALE) {
            (((unsigned char**)(out -> pixels))[j]) = (unsigned char *)malloc(out -> width);
        }
        else {
            ((unsigned char***)(out -> pixels))[j] = (unsigned char **)malloc(out -> width * 3 * sizeof(unsigned char));
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
                irf = i * resize_factor;
                sumR = sumG = sumB = 0;
                i0 = 3 * i;
                i1 = i0 + 1;
                i2 = i0 + 2;

                for (a = 0; a < resize_factor; a++) {
                    jrfa = jrf + a;

                    for (b = 0; b < resize_factor; b++) {
                        irfb = 3 * irf + b * 3;
                        irfb1 = irfb + 1;
                        irfb2 = irfb + 2;

                        sumR += ((unsigned char**)(in -> pixels))[jrfa][irfb];
                        sumG += ((unsigned char**)(in -> pixels))[jrfa][irfb1];
                        sumB += ((unsigned char**)(in -> pixels))[jrfa][irfb2];
                    }
                }

                ((unsigned char**)(out -> pixels))[j][i0] = sumR / rf_2;
                ((unsigned char**)(out -> pixels))[j][i1] = sumG / rf_2;
                ((unsigned char**)(out -> pixels))[j][i2] = sumB / rf_2;
            }
        }
        else if (out -> type == IMAGE_COLOR && resize_factor == 3) {
            for (i = 0; i < out -> width; i++) {
                i0 = 3 * i;
                i1 = i0 + 1;
                i2 = i0 + 2;

                irf = (i * resize_factor) * 3;
                irf1 = irf + 3;
                irf2 = irf + 6;

                ((unsigned char**)(out -> pixels))[j][i0] = (
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

                irf += 1;
                irf1 += 1;
                irf2 += 1;

                ((unsigned char**)(out -> pixels))[j][i1] = (
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
                
                irf += 1;
                irf1 += 1;
                irf2 += 1;

                ((unsigned char**)(out -> pixels))[j][i2] = (
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