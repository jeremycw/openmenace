#ifndef GFX_H
#define GFX_H

#include <stdint.h>

#include "gfx_common.h"

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
int gfx_graphic_iter(struct gfx *gfx, enum gfx_type graphic_type,
                     struct gfx_ega_graphic *graphic, int *iter);
void gfx_destroy(struct gfx *gfx);

#endif
