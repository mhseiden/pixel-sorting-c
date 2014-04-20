#ifndef _READ_WRITE_H
#define _READ_WRITE_H

struct Pixel {
	unsigned char *data;
	int components;
};

struct Image;

struct Image * read_image(const char * const);
void write_image(struct Image *, const char * const);

int get_width(struct Image *);
int get_height(struct Image *);
int get_components(struct Image *);
struct Pixel * get_pixels(struct Image *);

#endif
