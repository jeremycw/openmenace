#ifndef MAP_H
#define MAP_H

struct maps;

struct maps *maps_create(char const *head_path, char const *map_path);
void maps_print(struct maps *maps);
void maps_destroy(struct maps *maps);

#endif // MAP_H
