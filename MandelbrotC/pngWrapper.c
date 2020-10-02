/*
	Wrapper around LibPNG 
	Source : https://gist.github.com/1995eaton/1fd160fa776f31044b40
*/


#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "pngWrapper.h"

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

typedef struct Pixel {

};

int put_pixel(PNG* png, int x, int y) {
    if (x < 0 || y < 0 || x >= png->width || y >= png->height) {
        return -1;
    }
    png_bytep pixel = &(png->data[y][x * png->pixel_bytes]);
    pixel[0] = png->color[0];
    pixel[1] = png->color[1];
    pixel[2] = png->color[2];
    if (png->pixel_bytes == 4) {
        pixel[3] = 255;
    }
    return 0;
}

void write_png(PNG* png, char* fname) {
    FILE* fp = fopen(fname, "wb");
    png->pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);
    png->infop = png_create_info_struct(png->pngp);
    png_init_io(png->pngp, fp);
    png_set_IHDR(png->pngp, png->infop, png->width, png->height,
        png->depth, png->color_type, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png->pngp, png->infop);
    png_write_image(png->pngp, png->data);
    png_write_end(png->pngp, NULL);
    fclose(fp);
}

void set_color(PNG* png, png_byte r, png_byte g, png_byte b) {
    png->color[0] = r;
    png->color[1] = g;
    png->color[2] = b;
    if (png->pixel_bytes == 4) {
        png->color[3] = 255;
    }
}

int read_png(PNG* png, char* fname) {
    FILE* fp = fopen(fname, "rb");
    char header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp((unsigned char*)header, 0, 8)) {
        return -1;
    }
    png->pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);
    png->infop = png_create_info_struct(png->pngp);
    png_init_io(png->pngp, fp);
    png->pixel_bytes = 3;
    png_set_sig_bytes(png->pngp, 8);
    png_read_info(png->pngp, png->infop);
    png->width = png_get_image_width(png->pngp, png->infop);
    png->height = png_get_image_height(png->pngp, png->infop);
    png->color_type = png_get_color_type(png->pngp, png->infop);
    png->depth = png_get_bit_depth(png->pngp, png->infop);
    png_read_update_info(png->pngp, png->infop);
    png->data = malloc(png->height * sizeof(png_bytep));
    png->color = malloc(3 * sizeof(png_byte));
    set_color(png, 255, 255, 255);
    for (int y = 0; y < png->height; y++) {
        png->data[y] = malloc(png->pixel_bytes * png->width * sizeof(png_byte));
    }
    png_read_image(png->pngp, png->data);
    fclose(fp);
    return 0;
}

void create_png(PNG* png, int width, int height, png_byte color_mode) {
    png->width = width;
    png->height = height;
    png->pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);
    png->infop = png_create_info_struct(png->pngp);
    png->depth = 8;
    png->color_type = color_mode;
    switch (png->color_type) {
    case PNG_COLOR_TYPE_RGBA:
        png->pixel_bytes = 4;
        break;
    case PNG_COLOR_TYPE_RGB:
        png->pixel_bytes = 3;
        break;
    }
    png_set_IHDR(png->pngp, png->infop, width, height,
        png->depth, png->color_type, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_DEFAULT);
    png->data = malloc(height * sizeof(png_bytep));
    png->color = malloc(3 * sizeof(png_byte));
    set_color(png, 255, 255, 255);
    for (int y = 0; y < height; y++) {
        png->data[y] = malloc(png->pixel_bytes * width * sizeof(png_byte));
    }
}

void draw_rect(PNG* png, int x, int y, int width, int height) {
    for (int yy = y; yy <= height; yy++) {
        for (int xx = x; xx <= width; xx++) {
            put_pixel(png, xx, yy);
        }
    }
}