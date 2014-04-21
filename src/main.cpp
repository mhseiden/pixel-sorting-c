#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "../include/read_write.h"
#include "../include/sorting_context.h"
#include "../include/sorting.h"

#define ARG_DARK "dark"
#define ARG_LIGHT "light"
#define ARG_ALL "all"

#define ARG_AVG "avg"
#define ARG_MUL "mul"
#define ARG_MAX "max"
#define ARG_MIN "min"
#define ARG_XOR "xor"

using namespace std;

int main(const int argc, const char* argv[]) {
	if(argc != 5) {
		printf("usage: pixelsort [all|dark threshold|light threshold] [avg|mul|max|min|xor] [src.jpg] [dest.jpg]\n");
		return 1;
	}

	// Set up the argument strings
	const char * method = argv[1];
	const char * comparator = argv[2];
	const char * source = argv[3];
	const char * destination = argv[4];

	printf("Using method: %s\n", method);
	printf("Using comparator: %s\n", comparator);
	printf("Using source: %s\n", source);
	printf("Using destination: %s\n", destination);

	struct Image * image = read_image(source);
	struct PixelSortingContext * ctx = create_context();	

	set_orientation(ctx, ROW);
	set_sort_direction(ctx, ASC);


	if(0 == strcmp(ARG_DARK, method)) {
		set_run_type(ctx, DARK);
		set_threshold(ctx, 45);
	} else if(0 == strcmp(ARG_LIGHT, method)) {
		set_run_type(ctx, LIGHT);
		set_threshold(ctx, 210);
	} else {
		set_run_type(ctx, ALL);
	}

	if(0 == strcmp(ARG_AVG, comparator)) {
		set_comparison(ctx, AVG);
	} else if(0 == strcmp(ARG_MUL, comparator)) {
		set_comparison(ctx, MUL);
	} else if(0 == strcmp(ARG_MAX, comparator)) {
		set_comparison(ctx, MAX);
	} else if(0 == strcmp(ARG_MIN, comparator)) {
		set_comparison(ctx, MIN);
	} else {
		set_comparison(ctx, XOR);
	}

	sort(image, ctx);

	write_image(image, destination);
}
