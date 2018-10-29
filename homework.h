#ifndef HOMEWORK_H
#define HOMEWORK_H

#define IMAGE_GRAYSCALE 5
#define IMAGE_COLOR 6

typedef struct {
    void * pixels;
    int width, height;
    unsigned char type, max;
} image;

typedef struct {
    image * in, * out;
    int start, stop;
} options;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */