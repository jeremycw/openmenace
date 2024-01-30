#include <stdio.h>
#include <stdlib.h>

#include "maphead.h"
#include "readfile.h"

struct maphead {
  uint16_t magic;
  int32_t ptr[100];
};

struct maphead *maphead_create(char const *path) {
  struct maphead *maphead = calloc(sizeof(struct maphead), 1);
  if (maphead == NULL) {
    return NULL;
  }

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    goto error;
  }

  maphead->magic = read_uint16(file);

  for (int i = 0; i < 100; i++) {
    maphead->ptr[i] = read_int32(file);
  }

  fclose(file);

  return maphead;

error:
  maphead_destroy(maphead);
  return NULL;
}

int maphead_iter(struct maphead *maphead, int32_t *ptr, int *iter) {
  do {
    if (*iter >= 100) {
      return 0;
    }

    *ptr = maphead->ptr[*iter];
    *iter += 1;
  } while (*ptr == 0 || *ptr == -1);

  return 1;
}

int maphead_level_count(struct maphead *maphead) {
  int count = 0;
  int iter = 0;
  int32_t ptr;
  while (maphead_iter(maphead, &ptr, &iter)) {
    count++;
  }
  return count;
}

void maphead_destroy(struct maphead *maphead) { free(maphead); }
