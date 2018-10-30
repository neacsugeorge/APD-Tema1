#include "homework1.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

int num_threads;
int resolution;

void initialize(image *im) {
    im -> width = resolution;
    im -> height = resolution;

    im -> pixels = (unsigned char **)malloc(im -> height * sizeof(unsigned char *));

    int j;
    for (j = 0; j < im -> height; j++) {
        (((unsigned char**)(im -> pixels))[j]) = (unsigned char *)calloc(im -> width, 1);
    }
}

void * threadFunction (void * var) {
    options opt = *(options*)var;

    image * img = opt.img;

    int i, j;
    for (j = opt.start; j < opt.stop; j++) {
        for (i = 0; i < img -> width; i++) {
            int y = img -> width - i - 1, x = j;
            
            // CEA MAI DE CACAT CHESTIE EVER, sa ajungi sa faci bruteforce pe aceeasi formula
            // pana cand aproximarea se nimereste cu ce aveti in checker...
            if (abs((-1.0 * (j + 0.5) + 2.0 * (i + 0.5)) / (img -> width / 100)) / sqrt(5) < 3) {
                ((unsigned char **)(img -> pixels))[y][x] = 255;
            }
        }
    }
}

void render(image *im) {
    
    pthread_t tid[num_threads];
    options opt[num_threads];

    int i;
    for (i = 0; i < num_threads; i++) {
        opt[i].img = im;
        opt[i].start = (im -> height / num_threads) * i;
        opt[i].stop = (im -> height / num_threads) * (i+1);

        if (i == num_threads - 1) {
            opt[i].stop = im -> height;
        }
    }

    for (i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(opt[i]));
    }

    for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

}

void writeData(const char * fileName, image *img) {
    FILE * file;

    file = fopen(fileName, "w");

    fprintf(
        file,
        "P%hhu\n%d %d\n%hhu\n",
        5,
        img -> width,
        img -> height,
        255
    );

    int i, j;
    for (j = 0; j < img -> height; j++) {
        for (i = 0; i < img -> width; i++) {
            fprintf(
                file,
                "%c",
                !(((unsigned char **)(img -> pixels))[j][i]) ? 255 : 0
            );
        }
    }

    fclose(file);
}

