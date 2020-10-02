/*
    Wrapper around LibPNG
    Source : https://gist.github.com/1995eaton/1fd160fa776f31044b40
*/

#include "png.h"

typedef struct PNG {
    int width;
    int height;
    int depth;
    int pixel_bytes;
    png_bytep color;
    png_structp pngp;
    png_byte color_type;
    png_infop infop;
    png_byte** data;
} PNG;

int put_pixel(PNG* png, int x, int y);
void write_png(PNG* png, char* fname);

void set_color(PNG* png, png_byte r, png_byte g, png_byte b);

int read_png(PNG* png, char* fname);

void create_png(PNG* png, int width, int height, png_byte color_mode);
void draw_rect(PNG* png, int x, int y, int width, int height);
