#ifndef MAP_H
#define MAP_H

struct map;

struct map* map_create(char const* path);
void map_print(struct map* map);
void map_destroy(struct map* map);

#endif // MAP_H
