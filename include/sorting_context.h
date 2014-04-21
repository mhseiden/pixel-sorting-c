#ifndef _SORTING_CONTEXT_H
#define _SORTING_CONTEXT_H

struct PixelSortingContext;

enum Orientation_e { COLUMN, ROW, BOTH };
enum RunType_e { ALL, DARK, LIGHT };
enum Comparison_e { AVG, MUL, MAX, MIN, XOR };
enum SortDirection_e { ASC, DESC };

struct PixelSortingContext * create_context();
void destroy_context(struct PixelSortingContext *);

void set_orientation(struct PixelSortingContext *, Orientation_e);
Orientation_e get_orientation(const struct PixelSortingContext *);

void set_run_type(struct PixelSortingContext *, RunType_e);
RunType_e get_run_type(const struct PixelSortingContext *);

void set_comparison(struct PixelSortingContext *, Comparison_e);
Comparison_e get_comparison(const struct PixelSortingContext *);

void set_sort_direction(struct PixelSortingContext *, SortDirection_e);
SortDirection_e get_sort_direction(const struct PixelSortingContext *);

void set_threshold(struct PixelSortingContext *, long);
long get_threshold(const struct PixelSortingContext *);

#endif
