#include "../include/read_write.h"
#include "../include/parser.h"

#include <cstdlib>
#include <cstdio>
#include <cassert>

#define COMPONENTS 3

#define CMP_FN inline int
#define VAL_FN inline int

#define AVG_VAL avg_val
#define MUL_VAL mul_val
#define MAX_VAL max_val
#define MIN_VAL min_val
#define XOR_VAL orx_val

#define AVG_CMP avg_cmp
#define MUL_CMP mul_cmp
#define MAX_CMP max_cmp
#define MIN_CMP min_cmp
#define XOR_CMP orx_cmp

#define NOT_AVG_CMP n_avg_cmp
#define NOT_MUL_CMP n_mul_cmp
#define NOT_MAX_CMP n_max_cmp
#define NOT_MIN_CMP n_min_cmp
#define NOT_XOR_CMP n_orx_cmp

struct SortPlan;

typedef struct PixelSortQuery PixelSortQuery_t;

typedef struct Pixel {
	const unsigned char r;
	const unsigned char g; 
	const unsigned char b; 
} Pixel_t;

// Sorting Function Typedefs
typedef int(*sort_val_fn_t)(const Pixel_t *);
typedef int(*compare_fn_t)(const Pixel_t *, const Pixel_t *);
typedef void(*run_processor_fn_t)(Pixel_t *, const struct SortPlan *);

struct SortPlan;

typedef struct SortPlan {
	int run_count;
	int run_length;
	int is_ascending;
	long threshold;

	Orientation_e orientation;

	run_processor_fn_t run_processor_fn;	
	sort_val_fn_t sort_val_fn;
	compare_fn_t compare_fn;
} SortPlan_t;

// Sorter
void sort_run(Pixel_t *, const int, compare_fn_t);

// Run Processors
void dark_run_processor(Pixel_t *, const SortPlan_t *);
void light_run_processor(Pixel_t *, const SortPlan_t *);
void fixed_run_processor(Pixel_t *, const SortPlan_t *);
void default_run_processor(Pixel_t *, const SortPlan_t *);

// Run Detectors
int get_first_dark(const Pixel_t *, sort_val_fn_t, const int, const long);
int get_first_non_dark(const Pixel_t *, sort_val_fn_t, const int, const long);
int get_first_light(const Pixel_t *, sort_val_fn_t, const int, const long);
int get_next_fixed_end(const Pixel_t *, const int, const int);
int get_first_non_light(const Pixel_t *, sort_val_fn_t, const int, const long);

// Sort Value Extractors
static int AVG_VAL(const Pixel_t *);
static int MUL_VAL(const Pixel_t *);
static int MAX_VAL(const Pixel_t *);
static int MIN_VAL(const Pixel_t *);
static int XOR_VAL(const Pixel_t *);

// Comparison Funtions
static int AVG_CMP(const Pixel_t *, const Pixel_t *);
static int MUL_CMP(const Pixel_t *, const Pixel_t *);
static int MAX_CMP(const Pixel_t *, const Pixel_t *);
static int MIN_CMP(const Pixel_t *, const Pixel_t *);
static int XOR_CMP(const Pixel_t *, const Pixel_t *);

// Negated Comparison Funtions
static int NOT_AVG_CMP(const Pixel_t *, const Pixel_t *);
static int NOT_MUL_CMP(const Pixel_t *, const Pixel_t *);
static int NOT_MAX_CMP(const Pixel_t *, const Pixel_t *);
static int NOT_MIN_CMP(const Pixel_t *, const Pixel_t *);
static int NOT_XOR_CMP(const Pixel_t *, const Pixel_t *);

/**
 * Create a list of Pixel_t objects using the given Image and PixelSortingContext
 */
static Pixel_t * create_pixel_list(const struct Image * const, const SortPlan_t *);

/**
 * Sync the pixel list with the given image. ALWAYS call before returning from a sort call.
 */
static void sync_pixels(struct Image *, const SortPlan_t *, const Pixel_t *);

/**
 * Creates a sort plan with the given orientation
 */
static SortPlan_t * create_sort_plan(const Image *, const PixelSortQuery_t *, const size_t);

/**
 * Destroy the sort plan
 */
static void destroy_sort_plan(SortPlan_t *);

/**
 * Does the actual sort
 */
static void do_sort(Pixel_t *, const SortPlan_t *);

/**
 * Copies a char offset to a pixel
 */
static void set_pixel_from_buffer(const int, const unsigned char *, Pixel_t *);

/**
 * Copies a pixel to a byte offset
 */
static void set_buffer_from_pixel(const int, const Pixel_t *, unsigned char *);

void sort(struct Image * img, const PixelSortQuery_t * query) {
    for(int i = 0, l = get_subquery_count(query); i < l; ++i) {
	SortPlan_t * plan = create_sort_plan(img, query, i);

	Pixel_t * pixels = create_pixel_list(img, plan);
	do_sort(pixels, plan);

	sync_pixels(img, plan, pixels);
	destroy_sort_plan(plan);
    }
}

// pulls pixels out of the image, transposing the matrix if necessary
Pixel_t * create_pixel_list(const struct Image * const img, const SortPlan_t * plan_ptr) {
	const unsigned char * const buffer = get_buffer(img);
	const int width = get_width(img), height = get_height(img), components = get_components(img);
	assert(COMPONENTS == components);

	if(COLUMN == plan_ptr->orientation) {
		Pixel_t * pixels = (Pixel_t*)malloc(sizeof(Pixel_t) * width * height);
		for(int i = 0; i < width; ++i) {
			for(int j = 0; j < height; ++j) {
				int src_idx = ((j * width) + i);
				int dst_idx = ((i * height) + j);
				set_pixel_from_buffer(components, buffer + (src_idx * components), pixels + dst_idx);
			}
		}
		
		// free(buffer);
		return pixels;
	} else {
		return (Pixel_t*)buffer;
	}
}

// puts pixels back into the image, transposing the matrix if necessary
void sync_pixels(struct Image * img, const SortPlan_t * plan_ptr, const Pixel_t * pixels) {
	const int width = get_width(img), height = get_height(img), components = get_components(img);
	assert(COMPONENTS == components);

	if(COLUMN == plan_ptr->orientation) {
		unsigned char * const buffer = (unsigned char *)malloc(sizeof(unsigned char) * width * height * components);
		for(int i = 0; i < width; ++i) {
			for(int j = 0; j < height; ++j) {
				int src_idx = ((i * height) + j);
				int dst_idx = ((j * width) + i);
				unsigned char * data = buffer + (components * dst_idx);
				set_buffer_from_pixel(components, pixels + src_idx, data);
			}
		}

		// free(pixels);
		set_buffer(img, buffer);
	} else {
		// set_buffer(img, pixels);
	}
}

void destroy_sort_plan(SortPlan_t * plan_list_ptr) {
    free(plan_list_ptr);
}

SortPlan_t * create_sort_plan(const Image * img, const PixelSortQuery_t * query, const size_t subquery_idx) {
	debug_subquery(query, subquery_idx);

	SortPlan_t * plan = (SortPlan_t*)malloc(sizeof(SortPlan_t));
	const Orientation_e o = plan->orientation = get_orientation(query, subquery_idx);
	plan->is_ascending = (ASC == get_sort_direction(query, subquery_idx)) ? 1 : 0;
	plan->run_length = (ROW == o) ? get_width(img)  : get_height(img);
	plan->run_count	 = (ROW == o) ? get_height(img) : get_width(img);

	// Set the run type
	switch(get_run_type(query, subquery_idx)) {
		case DARK:
			plan->threshold = get_run_threshold(query, subquery_idx);
			plan->run_processor_fn = dark_run_processor;
			break;
		case LIGHT:
			plan->threshold = get_run_threshold(query, subquery_idx);
			plan->run_processor_fn = light_run_processor;
			break;
		case FIXED:
			plan->threshold = get_run_threshold(query, subquery_idx);
			plan->run_processor_fn = fixed_run_processor;
			break;
		case FULL:
		default:
			plan->run_processor_fn = default_run_processor;
			break;
	}

	// Set the comparison and sorting functions
	switch(get_comparison(query, subquery_idx)) {
		case AVG:
			plan->compare_fn = plan->is_ascending ? AVG_CMP : NOT_AVG_CMP;
			plan->sort_val_fn = AVG_VAL;
			break;
		case MUL:
			plan->compare_fn = plan->is_ascending ? MUL_CMP : NOT_MUL_CMP;
			plan->sort_val_fn = MUL_VAL;
			break;
		case MAX:
			plan->compare_fn = plan->is_ascending ? MAX_CMP : NOT_MAX_CMP;
			plan->sort_val_fn = MAX_VAL;
			break;
		case MIN:
			plan->compare_fn = plan->is_ascending ? MIN_CMP : NOT_MIN_CMP;
			plan->sort_val_fn = MIN_VAL;
			break;
		case XOR:
		default:
			plan->compare_fn = plan->is_ascending ? XOR_CMP : NOT_XOR_CMP;
			plan->sort_val_fn = XOR_VAL;
			break;
	}

	return plan;
}

void do_sort(Pixel_t * pixels, const SortPlan_t * plan) {
	for(int run = 0; run < plan->run_count; ++run) {
		(*plan->run_processor_fn)(pixels + (run * plan->run_length), plan);
	}
}

void dark_run_processor(Pixel_t * pixels, const SortPlan_t * plan_ptr) {
	const int length = plan_ptr->run_length, threshold = plan_ptr->threshold;
	Pixel_t * cursor = pixels;
	while(length > (cursor - pixels)) {
		Pixel_t * start = cursor + get_first_non_dark(cursor, plan_ptr->sort_val_fn, length - (cursor - pixels), threshold);
		Pixel_t * end = start + get_first_dark(start, plan_ptr->sort_val_fn, length - (start - pixels), threshold);
		sort_run(start, end - start, plan_ptr->compare_fn);
		cursor = end;
	}
}

void light_run_processor(Pixel_t * pixels, const SortPlan_t * plan) {
	const int length = plan->run_length, threshold = plan->threshold;
	Pixel_t * cursor = pixels;
	while(length > (cursor - pixels)) {
		Pixel_t * start = cursor + get_first_non_light(cursor, plan->sort_val_fn, length - (cursor - pixels), threshold);
		Pixel_t * end = start + get_first_light(start, plan->sort_val_fn, length - (start - pixels), threshold);
		sort_run(start, end - start, plan->compare_fn);
		cursor = end;
	}
}

void fixed_run_processor(Pixel_t * pixels, const SortPlan_t * plan) {
	const int length = plan->run_length, threshold = plan->threshold;
	Pixel_t * cursor = pixels;
	while(length > (cursor - pixels)) {
		Pixel_t * start = cursor;
		Pixel_t * end = start + get_next_fixed_end(start, length - (start - pixels), threshold);
		sort_run(start, end - start, plan->compare_fn);
		cursor = end;
	}
}

void default_run_processor(Pixel_t * pixels, const SortPlan_t * plan) {
	sort_run(pixels, plan->run_length, plan->compare_fn);
}

void sort_run(Pixel_t * start, const int length, compare_fn_t cmp) {
	qsort(start, length, sizeof(Pixel_t), (int(*)(const void*,const void*))cmp);
}

int get_next_fixed_end(const Pixel_t * pixels, const int remaining, const int interval) {
    return remaining > interval ? interval : remaining;
}

int get_first_dark(const Pixel_t * pixels, sort_val_fn_t v, const int length, const long threshold) {
	for(int idx = 0; idx < length; ++idx) {
		if(threshold >= (*v)(pixels + idx)) return idx;
	}
	return length;
}

int get_first_non_dark(const Pixel_t * pixels, sort_val_fn_t v, const int length, const long threshold) {
	for(int idx = 0; idx < length; ++idx) {
		if(threshold < (*v)(pixels + idx)) return idx;
	}
	return length;
}

int get_first_light(const Pixel_t * pixels, sort_val_fn_t v, const int length, const long threshold) {
	for(int idx = 0; idx < length; ++idx) {
		if(threshold <= (*v)(pixels + idx)) return idx;
	}
	return length;
}

int get_first_non_light(const Pixel_t * pixels, sort_val_fn_t v, const int length, const long threshold) {
	for(int idx = 0; idx < length; ++idx) {
		if(threshold > (*v)(pixels + idx)) return idx;
	}
	return length;
}

void set_pixel_from_buffer(const int components, const unsigned char * buffer, Pixel_t * pixel) {
	unsigned char * const as_buffer = (unsigned char *)pixel;
	for(int i = 0; i < components; ++i) {
	    *(as_buffer + i) = *(buffer + i);
	}
}

void set_buffer_from_pixel(const int components, const Pixel_t * pixel, unsigned char * buffer) {
	const unsigned char * const as_buffer = (unsigned char *)pixel;
	for(int i = 0; i < components; ++i) {
	    *(buffer + i) = *(as_buffer + i);
	}
}

/**
 * TODO: We can go farther with macros here, which might 
 * be nice if more value types are added.
 */

CMP_FN AVG_CMP(const Pixel_t * a, const Pixel_t * b) {
	return AVG_VAL(a) - AVG_VAL(b);
}

CMP_FN NOT_AVG_CMP(const Pixel_t * a, const Pixel_t * b) {
	return AVG_VAL(b) - AVG_VAL(a);
}


CMP_FN MUL_CMP(const Pixel_t * a, const Pixel_t * b) {
	return MUL_VAL(a) - MUL_VAL(b);
}

CMP_FN NOT_MUL_CMP(const Pixel_t * a, const Pixel_t * b) {
	return MUL_VAL(b) - MUL_VAL(a);
}


CMP_FN MAX_CMP(const Pixel_t * a, const Pixel_t * b) {
	return MAX_VAL(a) - MAX_VAL(b);
}

CMP_FN NOT_MAX_CMP(const Pixel_t * a, const Pixel_t * b) {
	return MAX_VAL(b) - MAX_VAL(a);
}


CMP_FN MIN_CMP(const Pixel_t * a, const Pixel_t * b) {
	return MIN_VAL(a) - MIN_VAL(b);
}

CMP_FN NOT_MIN_CMP(const Pixel_t * a, const Pixel_t * b) {
	return MIN_VAL(b) - MIN_VAL(a);
}


CMP_FN XOR_CMP(const Pixel_t * a, const Pixel_t * b) {
	return XOR_VAL(a) - XOR_VAL(b);
}

CMP_FN NOT_XOR_CMP(const Pixel_t * a, const Pixel_t * b) {
	return XOR_VAL(b) - XOR_VAL(a);
}


VAL_FN AVG_VAL(const Pixel_t * a) {
	int avg = 0;
	for(int c = 0, len = COMPONENTS; c < len; ++c) avg += ((unsigned char *)a)[c];
	return avg / COMPONENTS;
}

VAL_FN MUL_VAL(const Pixel_t * a) {
	int mul = 1;
	for(int c = 0, len = COMPONENTS; c < len; ++c) mul *= ((unsigned char *)a)[c];
	return mul;
}

VAL_FN MAX_VAL(const Pixel_t * a) {
	int max = -1;
	for(int c = 0, len = COMPONENTS; c < len; ++c) {
		if(((unsigned char *)a)[c] > max) max = ((unsigned char *)a)[c];
	}
	return max;
}

VAL_FN MIN_VAL(const Pixel_t * a) {
	int min = 256;
	for(int c = 0, len = COMPONENTS; c < len; ++c) {
		if(((unsigned char *)a)[c] < min) min = ((unsigned char *)a)[c];
	}
	return min;
}

VAL_FN XOR_VAL(const Pixel_t * a) {
	int orx = ((unsigned char *)a)[0];
	for(int c = 1, len = COMPONENTS; c < len; ++c) orx ^= ((unsigned char *)a)[c];
	return orx;
}
