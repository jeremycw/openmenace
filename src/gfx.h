#ifndef GFX_H
#define GFX_H

#include <stdint.h>

#include "gfx_pictures_pub.h"

struct gfx;

enum gfx_type {
  GFX_TYPE_UNMASKED_PICTURE,
  GFX_TYPE_MASKED_PICTURE,
  GFX_TYPE_UNMASKED_TILE_8,
  GFX_TYPE_MASKED_TILE_8,
  GFX_TYPE_UNMASKED_TILE_16,
  GFX_TYPE_MASKED_TILE_16
};

struct gfx *gfx_create(char const *head_path, char const *graph_path,
                       char const *dict_path);
void gfx_print(struct gfx *gfx);
int gfx_iterate_pictures(struct gfx *gfx, enum gfx_type picture_type,
                         struct gfx_picture **picture, int *iter);
int gfx_picture_count(struct gfx *gfx, enum gfx_type picture_type);
void gfx_destroy(struct gfx *gfx);

#endif
