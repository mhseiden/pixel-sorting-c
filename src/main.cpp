#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "../include/read_write.h"
#include "../include/sorting.h"
#include "../include/parser.h"

#define ARG_ROW "row"
#define ARG_COLUMN "column"
#define ARG_BOTH "both"

#define ARG_DARK "dark"
#define ARG_LIGHT "light"
#define ARG_ALL "all"

#define ARG_AVG "avg"
#define ARG_MUL "mul"
#define ARG_MAX "max"
#define ARG_MIN "min"
#define ARG_XOR "xor"

int main(const int argc, const char** argv) {

    if(argc != 4) {
	printf("example usage:  pixelsort [src.jpg] [dest.jpg] <pixelsort query>\n");
        printf("query syntax: SORT [ROWS|COLUMNS] [ASC|DESC] BY [AVG|MUL|MAX|MIN|XOR] WITH [FULL|DARK <THRESHOLD>|LIGHT <THRESHOLD>|FIXED <THRESHOLD>] RUNS [THEN SORT ...]\n");
        return 1;
    }

    const char* source		= argv[1];
    const char* destination	= argv[2];
    const char* query_string	= argv[3];

    struct PixelSortQuery * query = process_tokens(query_string);
    struct Image * image = read_image(source);
    sort(image, query);
    write_image(image, destination);
    destroy_query(query);
}
