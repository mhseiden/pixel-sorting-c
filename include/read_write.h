#ifndef _READ_WRITE_H
#define _READ_WRITE_H

struct Image;

struct Image * read_image(const char * const);
void write_image(struct Image *, const char * const);

int get_width(const struct Image * const);
int get_height(const struct Image * const);
int get_components(const struct Image * const);

const unsigned char * const get_buffer(const struct Image * const);
void set_buffer(struct Image *, unsigned char *);

#endif
