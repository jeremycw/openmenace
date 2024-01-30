#ifndef MAPTEMP_H
#define MAPTEMP_H

#include <stdint.h>

struct maptemp;
struct level;

struct maptemp *maptemp_create(char const *path);
void maptemp_destroy(struct maptemp *maptemp);

struct level *maptemp_level_create_from_ptr(struct maptemp *maptemp,
                                            int32_t level_ptr, int level_id);
void maptemp_level_print(struct level *level);
void maptemp_level_destroy(struct level *level);

#endif
