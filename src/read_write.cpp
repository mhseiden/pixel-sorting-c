#include "../include/read_write.h"

#include <iostream>

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>

#include "jpeglib.h"

using namespace std;

static const int STRIDES = 1;

static void sync_buffer(struct Image *);

typedef struct jpeg_decompress_struct jpeg_decompress_t;
typedef struct jpeg_compress_struct jpeg_compress_t;
typedef struct jpeg_error_mgr jpeg_error_mgr_t;

typedef struct Pixel Pixel_t;

typedef struct Image {
	unsigned char *buffer;
	int width;
	int height;
	int components;
} Image_t;

struct Image * read_image(const char * const file) {
	FILE * src;
	if(NULL == (src = fopen(file, "rb"))) {
		cout << "unable to open source file: " << file << endl;
	}

	Image_t * img = (Image_t*)malloc(sizeof(Image_t));

	// Create the structures
	jpeg_decompress_t d_info;
	jpeg_error_mgr_t jpg_err;

	// Init the error handler and the decompressor
	d_info.err = jpeg_std_error(&jpg_err);
	jpeg_create_decompress(&d_info);

	// Read the file and the header
	jpeg_stdio_src(&d_info, src);
	jpeg_read_header(&d_info, TRUE);

	// Start decompression
	jpeg_start_decompress(&d_info);

	// Create the img object we'll write into
	img->width = d_info.output_width;
	img->height = d_info.output_height;
	img->components = d_info.output_components;
	img->buffer = new unsigned char[img->width * img->height * img->components];

	// Create a sample row
	const int d_row_stride = img->width * img->components;
	JSAMPARRAY d_buf = (d_info.mem->alloc_sarray)((j_common_ptr)&d_info, JPOOL_IMAGE, d_row_stride, STRIDES);

	// Read in the lines
	for(int counter = 0; d_info.output_scanline < d_info.output_height; ++counter) {
		jpeg_read_scanlines(&d_info, d_buf, STRIDES);
		memcpy(img->buffer + (counter * img->width * img->components), *d_buf, d_row_stride);
	}

	// Finish decompression and release memory
	jpeg_finish_decompress(&d_info);
	jpeg_destroy_decompress(&d_info);
	fclose(src);

	cout << "width: " << img->width << " height: " << img->height << endl;

	return img;
}

void write_image(Image_t * img, const char * const file) {
	FILE * dest;
	if(NULL == (dest = fopen(file, "wb"))) {
		cout << "unable to open destination file: " << file << endl;
	}

	// Sync the pixel array with the buffer
	// sync_buffer(img);

	// Create the compressor structures
	jpeg_compress_t c_info;
	jpeg_error_mgr_t jpg_err;

	// Init the error handler and the decompressor
	c_info.err = jpeg_std_error(&jpg_err);
	jpeg_create_compress(&c_info);
	jpeg_stdio_dest(&c_info, dest);

	// Set the img properties
	c_info.image_width = img->width;
	c_info.image_height = img->height;
	c_info.input_components = img->components;
	c_info.in_color_space = JCS_RGB;

	jpeg_set_defaults(&c_info);
	jpeg_start_compress(&c_info, TRUE);

	// Create a sample row
	const int c_row_stride = img->width * img->components;
	JSAMPARRAY c_buf = (c_info.mem->alloc_sarray)((j_common_ptr)&c_info, JPOOL_IMAGE, c_row_stride, STRIDES);

	for(int counter = 0; c_info.next_scanline < img->height; ++counter) {
		memcpy(*c_buf, img->buffer + (counter * img->width * img->components), c_row_stride);
		jpeg_write_scanlines(&c_info, c_buf, STRIDES);
	}

	jpeg_finish_compress(&c_info);
	jpeg_destroy_compress(&c_info);
	fclose(dest);

	delete[] img->buffer;
	delete img;
}

void sync_buffer(Image_t * img) {
}

int get_width(const struct Image * const img) {
	return img->width;
}

int get_height(const struct Image * const img) {
	return img->height;
}

int get_components(const struct Image * const img) {
	return img->components;
}

const unsigned char * const get_buffer(const struct Image * const img) {
	return img->buffer;
}

void set_buffer(struct Image * img, unsigned char * buffer) {
	free(img->buffer);
	img->buffer = buffer;
}
