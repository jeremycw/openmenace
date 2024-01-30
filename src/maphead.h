#ifndef MAPHEAD_H
#define MAPHEAD_H

#include <stdint.h>

struct maphead;

struct maphead *maphead_create(char const *path);
int maphead_iter(struct maphead *maphead, int32_t *ptr, int *iter);
int maphead_level_count(struct maphead *maphead);
void maphead_destroy(struct maphead *maphead);

#endif
