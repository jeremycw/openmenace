#ifndef MAP_H
#define MAP_H

#include "map_common.h"

struct maps;

struct maps *maps_create(char const *head_path, char const *map_path);
void maps_print(struct maps *maps);
void maps_destroy(struct maps *maps);
struct map_plane maps_get_level_plane(struct maps *maps, int level_id, int plane_id);

#endif // MAP_H
