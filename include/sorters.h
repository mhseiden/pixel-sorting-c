#ifndef _SORTERS_H
#define _SORTERS_H

#include "read_write.h"

void dark_sorter(struct Image *, const long);
void light_sorter(struct Image *, const long);
void average_sorter(struct Image *);

#endif
