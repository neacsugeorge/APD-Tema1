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

int pxValue(unsigned char * values) {
    int j, sum = 0;

    if (resize_factor == 3) {
        sum = (
            values[0]
            + 2 * values[1]
            + values[2]
            + 2 * values[3]
            + 4 * values[4]
            + 2 * values[5]
            + values[6]
            + 2 * values[7]
            + values[8]
        );

        return sum / 16;
    }

    for (j = 0; j < resize_factor * resize_factor; j++) {
        sum += values[j];
    }

    return sum / (resize_factor * resize_factor);
}

void * threadFunction (void * var) {
    options opt = *(options*)var;

    image * in = opt.in;
    image * out = opt.out;

    int i, j, a, b;

    for (j = opt.start; j < opt.stop; j++) {
        for (i = 0; i < out -> width; i++) {
            unsigned char *x, *y, *z = (unsigned char *)malloc(resize_factor * resize_factor);
            

            if (out -> type == IMAGE_GRAYSCALE) {
                for (a = 0; a < resize_factor; a++) {
                    for (b = 0; b < resize_factor; b++) {
                        z[a * resize_factor + b] = ((unsigned char**)(in -> pixels))[j * resize_factor + a][i * resize_factor + b];
                    }
                }

                ((unsigned char**)(out -> pixels))[j][i] = pxValue(z);
            }
            else {
                x = (unsigned char *)malloc(resize_factor * resize_factor);
                y = (unsigned char *)malloc(resize_factor * resize_factor);

                for (a = 0; a < resize_factor; a++) {
                    for (b = 0; b < resize_factor; b++) {
                        x[a * resize_factor + b] = ((unsigned char***)(in -> pixels))[j * resize_factor + a][i * resize_factor + b][0];
                        y[a * resize_factor + b] = ((unsigned char***)(in -> pixels))[j * resize_factor + a][i * resize_factor + b][1];
                        z[a * resize_factor + b] = ((unsigned char***)(in -> pixels))[j * resize_factor + a][i * resize_factor + b][2];
                    }
                }

                ((unsigned char***)(out -> pixels))[j][i] = (unsigned char *)malloc(3 * sizeof(unsigned char));
                ((unsigned char***)(out -> pixels))[j][i][0] = pxValue(x);
                ((unsigned char***)(out -> pixels))[j][i][1] = pxValue(y);
                ((unsigned char***)(out -> pixels))[j][i][2] = pxValue(z);

                free(x);
                free(y);
            }

            free(z);
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

    int i, j;
    for (j = 0; j < out -> height; j++) {
        if (out -> type == IMAGE_GRAYSCALE) {
            (((unsigned char**)(out -> pixels))[j]) = (unsigned char *)malloc(out -> width);
        }
        else {
            ((unsigned char***)(out -> pixels))[j] = (unsigned char **)malloc(out -> width * sizeof(unsigned char *));
        }
    }

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