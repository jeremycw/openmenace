#include <stdio.h>
#include <stdlib.h>

#include "readfile.h"

#define RLE_MAGIC 0xABCD
#define PLANE_COUNT 3

struct lvlheader {
  uint32_t plane_start[PLANE_COUNT];
  uint16_t plane_length[PLANE_COUNT];
  uint16_t width, height;
  char name[16];
};

struct maptemp {
  FILE *file;
  struct lvlheader current_header;
};

struct level {
  struct lvlheader header;
  int chunk_id;
  int level_id;
  uint16_t *planes[PLANE_COUNT];
  uint16_t *buffer;
};

void lvlheader_read(struct lvlheader *header, FILE *file) {
  header->plane_start[0] = read_uint32(file);
  header->plane_start[1] = read_uint32(file);
  header->plane_start[2] = read_uint32(file);
  header->plane_length[0] = read_uint16(file);
  header->plane_length[1] = read_uint16(file);
  header->plane_length[2] = read_uint16(file);
  header->width = read_uint16(file);
  header->height = read_uint16(file);
  fread(header->name, sizeof(char), 16, file);
}

void lvlheader_print(struct lvlheader *header) {
  printf("Plane start: %d, %d, %d\n", header->plane_start[0],
         header->plane_start[1], header->plane_start[2]);
  printf("Plane length: %d, %d, %d\n", header->plane_length[0],
         header->plane_length[1], header->plane_length[2]);
  printf("Width: %d\n", header->width);
  printf("Height: %d\n", header->height);
  printf("Total size: %d\n", header->width * header->height);
  printf("Name: %s\n", header->name);
}

void CAL_RLEWExpand(void *src, void *dest, int expLength, uint16_t rletag) {
  uint16_t *srcptr = (uint16_t *)src;
  uint16_t *dstptr = (uint16_t *)dest;
  uint16_t count, value;

  while (expLength > 0) {
    value = *srcptr++;
    if (value != rletag) {
      *(dstptr++) = value;
      expLength -= 2;
    } else {
      count = *(srcptr++);
      value = *(srcptr++);
      expLength -= count * 2;
      if (expLength < 0) {
        return;
      }
      for (int i = 0; i < count; ++i) {
        *(dstptr++) = value;
      }
    }
  }
}

void maptemp_destroy(struct maptemp *maptemp) {
  if (maptemp == NULL) {
    return;
  }

  fclose(maptemp->file);
  free(maptemp);
}

struct maptemp *maptemp_create(char const *path) {
  struct maptemp *maptemp = calloc(sizeof(struct maptemp), 1);
  if (maptemp == NULL) {
    return NULL;
  }

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    goto error;
  }

  maptemp->file = file;

  return maptemp;

error:
  maptemp_destroy(maptemp);
  return NULL;
}

void maptemp_level_destroy(struct level *level) {
  if (level == NULL) {
    return;
  }

  free(level->buffer);
  free(level);
}

struct level *maptemp_level_create_from_ptr(struct maptemp *maptemp,
                                            int32_t level_ptr, int level_id) {
  fseek(maptemp->file, level_ptr, SEEK_SET);
  lvlheader_read(&maptemp->current_header, maptemp->file);

  struct level *level = malloc(sizeof(struct level));
  uint16_t *encoded_planes = NULL;

  if (level == NULL) {
    return NULL;
  }

  level->level_id = level_id;
  level->chunk_id = level_ptr;
  level->header = maptemp->current_header;

  level->buffer = malloc(sizeof(uint16_t) * level->header.width *
                         level->header.height * PLANE_COUNT);
  if (level->buffer == NULL) {
    goto error;
  }

  for (int i = 0; i < PLANE_COUNT; i++) {
    level->planes[i] =
        level->buffer + level->header.width * level->header.height * i;
  }

  int total_encoded_length = maptemp->current_header.plane_length[0] +
                             maptemp->current_header.plane_length[1] +
                             maptemp->current_header.plane_length[2];

  encoded_planes = malloc(sizeof(uint16_t) * total_encoded_length);
  if (encoded_planes == NULL) {
    goto error;
  }

  uint16_t *encoded_plane = encoded_planes;
  for (int i = 0; i < PLANE_COUNT; i++) {
    fseek(maptemp->file, maptemp->current_header.plane_start[i], SEEK_SET);
    int size = read_uint16(maptemp->file);
    fread(encoded_plane, sizeof(uint16_t),
          maptemp->current_header.plane_length[i], maptemp->file);
    CAL_RLEWExpand(encoded_plane, level->planes[i], size, RLE_MAGIC);
    encoded_plane = encoded_planes + maptemp->current_header.plane_length[i];
  }
  free(encoded_planes);

  return level;

error:
  free(encoded_planes);
  maptemp_level_destroy(level);
  return NULL;
}

void maptemp_level_print(struct level *level) {
  lvlheader_print(&level->header);
  for (int i = 0; i < PLANE_COUNT; i++) {
    printf("Plane %d:\n", i);
    for (int y = 0; y < level->header.height; y++) {
      for (int x = 0; x < level->header.width; x++) {
        printf("%d ", level->planes[i][y * level->header.width + x]);
      }
      printf("\n");
    }
  }
}
