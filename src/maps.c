#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "maphead.h"
#include "maps.h"
#include "maptemp.h"

struct maps {
  int n;
  struct level *buffer[0];
};

struct maps *maps_create(char const *head_path, const char *map_path) {
  struct maphead *maphead = NULL;
  struct maptemp *maptemp = NULL;
  struct maps *maps = NULL;

  maphead = maphead_create(head_path);
  if (maphead == NULL) {
    goto error;
  }

  int level_count = maphead_level_count(maphead);
  maps = calloc(sizeof(struct maps) + level_count * sizeof(struct level *), 1);
  if (maps == NULL) {
    goto error;
  }

  maptemp = maptemp_create(map_path);
  if (maptemp == NULL) {
    goto error;
  }

  int iter = 0;
  int32_t ptr;
  while (maphead_iter(maphead, &ptr, &iter)) {
    struct level *level = maptemp_level_create_from_ptr(maptemp, ptr, iter - 1);
    if (level == NULL) {
      goto error;
    }
    maps->buffer[maps->n++] = level;
  }

  maptemp_destroy(maptemp);
  maphead_destroy(maphead);

  return maps;

error:
  maptemp_destroy(maptemp);
  maphead_destroy(maphead);
  maps_destroy(maps);
  return NULL;
}

void maps_destroy(struct maps *maps) {
  if (maps == NULL) {
    return;
  }

  for (int i = 0; i < maps->n; ++i) {
    maptemp_level_destroy(maps->buffer[i]);
  }
  free(maps);
}

void maps_print(struct maps *maps) {
  for (int i = 0; i < maps->n; ++i) {
    maptemp_level_print(maps->buffer[i]);
  }
}
