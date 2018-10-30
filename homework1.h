#ifndef HOMEWORK_H1
#define HOMEWORK_H1

typedef struct {
    void * pixels;
    int width, height;
} image;


typedef struct {
    image * img;
    int start, stop;
} options;

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */