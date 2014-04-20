#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cassert>

#include "../include/read_write.h"
#include "../include/sorters.h"

using namespace std;

typedef struct Pixel * Pixel_ptr_t;

typedef void * processor_arg_t;
typedef int(*pixel_cmp)(Pixel_ptr_t, const struct Pixel *);
typedef void(*line_processor_t)(Pixel_ptr_t, const int, const processor_arg_t);

static void process_lines(struct Image *, line_processor_t, processor_arg_t);
static void dark_line_processor(Pixel_ptr_t, const int, int);
static void light_line_processor(Pixel_ptr_t, const int, int);
static void average_line_processor(Pixel_ptr_t, const int, void*);

static void sort_line(Pixel_ptr_t, const int, pixel_cmp);
static int avg_cmp(const Pixel_ptr_t, const Pixel_ptr_t);

static int get_first_dark(Pixel_ptr_t, const int, const long);
static int get_first_non_dark(Pixel_ptr_t, const int, const long);

static int get_first_light(Pixel_ptr_t, const int, const long);
static int get_first_non_light(Pixel_ptr_t, const int, const long);

static unsigned char avg(Pixel_ptr_t);

void dark_sorter(struct Image * img, const long threshold) {
	process_lines(img, (line_processor_t)dark_line_processor, (processor_arg_t)threshold);
}

void light_sorter(struct Image * img, const long threshold) {
	process_lines(img, (line_processor_t)light_line_processor, (processor_arg_t)threshold);
}

void average_sorter(struct Image * img) {
	process_lines(img, (line_processor_t)average_line_processor, NULL);
}

void process_lines(struct Image * img, line_processor_t proc, processor_arg_t arg) {
	Pixel_ptr_t pixels = get_pixels(img);
	const int width = get_width(img), height = get_height(img);

	for(int line = 0; line < height; ++line) {
		(*proc)(pixels + (width * line), width, arg);
	}
}

void dark_line_processor(Pixel_ptr_t line, const int length, int threshold) {
	Pixel_ptr_t cursor = line;
	while(length > (cursor - line)) {
		Pixel_ptr_t start = cursor + get_first_non_dark(cursor, length - (cursor - line), threshold);
		Pixel_ptr_t end = start + get_first_dark(start, length - (start - line), threshold);
		sort_line(start, end - start, (pixel_cmp)avg_cmp);
		cursor = end;
	}
}

void light_line_processor(Pixel_ptr_t line, const int length, int threshold) {
	Pixel_ptr_t cursor = line;
	while(length > (cursor - line)) {
		Pixel_ptr_t start = cursor + get_first_non_light(cursor, length - (cursor - line), threshold);
		Pixel_ptr_t end = start + get_first_light(start, length - (start - line), threshold);
		sort_line(start, end - start, (pixel_cmp)avg_cmp);
		cursor = end;
	}
}

void average_line_processor(Pixel_ptr_t ptr, const int length, void * ignore) {
	sort_line(ptr, length, (pixel_cmp)avg_cmp);
}

int get_first_dark(Pixel_ptr_t line, const int length, const long threshold) {
	int idx;
	for(idx = 0; idx < length; ++idx) {
		if(threshold >= avg(line + idx)) {
			break;
		}
	}
	return idx;
}

int get_first_non_dark(Pixel_ptr_t line, const int length, const long threshold) {
	int idx;
	for(idx = 0; idx < length; ++idx) {
		if(threshold < avg(line + idx)) {
			break;
		}
	}
	return idx;
}

int get_first_light(Pixel_ptr_t line, const int length, const long threshold) {
	int idx;
	for(idx = 0; idx < length; ++idx) {
		if(threshold <= avg(line + idx)) {
			break;
		}
	}
	return idx;
}

int get_first_non_light(Pixel_ptr_t line, const int length, const long threshold) {
	int idx;
	for(idx = 0; idx < length; ++idx) {
		if(threshold > avg(line + idx)) {
			break;
		}
	}
	return idx;
}

void sort_line(Pixel_ptr_t line, const int length, pixel_cmp cmp) {
	qsort(line, length, sizeof(struct Pixel), (int(*)(const void*, const void*))cmp);
}

int avg_cmp(const Pixel_ptr_t a, const Pixel_ptr_t b) {
	assert(a->components == b->components);
	int a_agg = 0, b_agg = 0;
	for(int c = 0, len = a->components; c < len; ++c) {
		a_agg += a->data[c];
		b_agg += b->data[c];
	}
	return a_agg - b_agg; // We don't actually need the AVG, since scaling will not change the sign
}

unsigned char avg(Pixel_ptr_t a) {
	int avg = 0;
	for(int c = 0, len = a->components; c < len; ++c) avg += a->data[c];
	return (unsigned char)(avg / a->components);
}

/*

// Sort pixels in a line based on the avg value
int avg_cmp(const Pixel_t *a, const Pixel_t *b) {
}

void sort_line(Pixel_t * line, const int length, const pixel_sort_cmp_t cmp) {
	qsort(line, length, sizeof(Pixel_t), (int(*)(const void*, const void*))cmp);
}

int get_first_light(Pixel_t * line, const int length) {
	int idx;
	for(idx = 0; idx < length; ++idx) {
		if(LIGHT_VALUE <= pixel_avg(line + idx)) {
			break;
		}
	}
	return idx;
}

int get_first_non_light(Pixel_t * line, const int length) {
	int idx;
	for(idx = 0; idx < length; ++idx) {
		if(LIGHT_VALUE > pixel_avg(line + idx)) {
			break;
		}
	}
	return idx;
}

void sort_full_line(Pixel_t * line, const int length) {
	sort_line(line, length, avg_cmp);
}

void sort_by_dark_runs(Pixel_t * line, const int length) {
}

void sort_by_light_runs(Pixel_t * line, const int length) {
	Pixel_t * cursor = line;
	while(length > (cursor - line)) {
		Pixel_t * start = cursor + get_first_non_light(cursor, length - (cursor - line));
		Pixel_t * end = start + get_first_light(start, length - (start - line));
		sort_line(start, end - start, avg_cmp);
		cursor = end;
	}
}

void process_line(Pixel_t * line, const int length, const pixel_line_proc_t proc) {
	proc(line, length);
}

*/

