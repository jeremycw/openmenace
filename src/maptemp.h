#ifndef MAPTEMP_H
#define MAPTEMP_H

#include <stdint.h>

#include "map_common.h"

struct maptemp;
struct level;

struct maptemp *maptemp_create(char const *path);
void maptemp_destroy(struct maptemp *maptemp);

struct level *maptemp_level_create_from_ptr(struct maptemp *maptemp,
                                            int32_t level_ptr, int level_id);
void maptemp_level_print(struct level *level);
int maptemp_level_get_id(struct level *level);
struct map_plane maptemp_level_get_plane(struct level *level, int plane_id);
void maptemp_level_destroy(struct level *level);

#endif
