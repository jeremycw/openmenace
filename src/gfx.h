#ifndef GFX_H
#define GFX_H

struct gfx;

struct gfx *gfx_create(char const *head_path, char const *graph_path,
                       char const *dict_path);
void gfx_print(struct gfx *gfx);
void gfx_destroy(struct gfx *gfx);

#endif
