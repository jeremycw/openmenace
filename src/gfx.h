#ifndef GFX_H
#define GFX_H

#include "gfx_common.h"

struct gfx;

struct gfx *gfx_create(char const *head_path, char const *graph_path,
                       char const *dict_path);
void gfx_print(struct gfx *gfx);
int gfx_picture_iter(struct gfx* gfx, enum gfx_picture_type picture_type, int *picture_id, int *iter);
void gfx_destroy(struct gfx *gfx);

#endif
