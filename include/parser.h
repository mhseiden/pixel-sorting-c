#ifndef _PARSER_H
#define _PARSER_H

#include <cstdlib>

enum Orientation_e { COLUMN, ROW };
enum RunType_e { FULL, DARK, LIGHT, FIXED };
enum Comparison_e { AVG, MUL, MAX, MIN, XOR };
enum SortDirection_e { ASC, DESC };

struct PixelSortQuery;

// construction and destruction
struct PixelSortQuery * process_tokens(const char *);
void destroy_query(struct PixelSortQuery *);

// accessor methods (high level and per-subquery)
int get_subquery_count(const struct PixelSortQuery *);
void debug_subquery(const struct PixelSortQuery *, const int);
long get_run_threshold(const struct PixelSortQuery *, const int);
Orientation_e get_orientation(const struct PixelSortQuery *, const int);
RunType_e get_run_type(const struct PixelSortQuery *, const int);
Comparison_e get_comparison(const struct PixelSortQuery *, const int);
SortDirection_e get_sort_direction(const struct PixelSortQuery *, const int);

#endif
