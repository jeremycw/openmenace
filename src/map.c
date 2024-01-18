#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "readfile.h"
#include "map.h"

struct maptype {
	uint32_t planestart[3];
	uint16_t planelength[3];
	uint16_t width,height;
	char name[16];
};

struct map {
  struct maptype maptype;
  uint16_t* plane[3];
  uint16_t width,height;
};

void maptype_read(struct maptype* maptype, FILE* file) {
  char header[8];
  fread(header, sizeof(char), 8, file);
  maptype->planestart[0] = read_uint32(file);
  maptype->planestart[1] = read_uint32(file);
  maptype->planestart[2] = read_uint32(file);
  maptype->planelength[0] = read_uint16(file);
  maptype->planelength[1] = read_uint16(file);
  maptype->planelength[2] = read_uint16(file);
  maptype->width = read_uint16(file);
  maptype->height = read_uint16(file);
  fread(maptype->name, sizeof(char), 16, file);
}

void maptype_print(struct maptype* maptype) {
  printf("Plane start: %d, %d, %d\n", maptype->planestart[0], maptype->planestart[1], maptype->planestart[2]);
  printf("Plane length: %d, %d, %d\n", maptype->planelength[0], maptype->planelength[1], maptype->planelength[2]);
  printf("Width: %d\n", maptype->width);
  printf("Height: %d\n", maptype->height);
  printf("Total size: %d\n", maptype->width * maptype->height);
  printf("Name: %s\n", maptype->name);
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

struct map* map_create(char const* path) {
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    printf("Error opening file\n");
    return NULL;
  }

  struct map* map = malloc(sizeof(struct map));
  if (map == NULL) {
    goto error1;
  }

  maptype_read(&map->maptype, file);

  map->width = map->maptype.width;
  map->height = map->maptype.height;
  for (int i = 0; i < 3; ++i) {
    map->plane[i] = malloc(sizeof(uint16_t) * map->width * map->height);
    if (map->plane[i] == NULL) {
      goto error2;
    }
  }

  // read planes
  for (int i = 0; i < 3; ++i) {
    uint16_t* encoded = malloc(sizeof(uint16_t) * map->maptype.planelength[i]);
    if (encoded == NULL) {
      goto error2;
    }
    fseek(file, map->maptype.planestart[i], SEEK_SET);
    fread(encoded, sizeof(uint16_t), map->maptype.planelength[i], file);
    CAL_RLEWExpand(encoded, map->plane[i], map->height * map->width, 0xABCD);
    free(encoded);
  }

  fclose(file);

  return map;

error2:
  for (int i = 0; i < 3; ++i) {
    free(map->plane[i]);
  }
  free(map);
error1:
  fclose(file);
  return NULL;
}

void map_print(struct map* map) {
  maptype_print(&map->maptype);

  for (int i = 0; i < 3; ++i) {
    printf("Plane %d:\n", i);
    for (int y = 0; y < map->height; ++y) {
      for (int x = 0; x < map->width; ++x) {
        printf("%04X ", map->plane[i][y * map->width + x]);
      }
      printf("\n");
    }
  }
}

void map_destroy(struct map* map) {
  for (int i = 0; i < 3; ++i) {
    if (map->plane[i] != NULL) {
      free(map->plane[i]);
    }
  }
  free(map);
}
