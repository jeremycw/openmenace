#ifndef GFX_PICTURES_H
#define GFX_PICTURES_H

#include <stdint.h>

struct gfx_pictures;

struct gfx_pictures *gfx_pictures_create_from_table(uint16_t *table, int size,
                                                    struct gfx_decoder *decoder,
                                                    int numplanes);
struct gfx_pictures *
gfx_pictures_create_from_8x8_tile_chunk(struct gfx_decoder *decoder,
                                        int numplanes);
struct gfx_pictures *gfx_pictures_create_from_tiles(struct gfx_decoder *decoder,
                                                    int tile_count,
                                                    int tile_size,
                                                    int numplanes);
struct gfx_picture* gfx_pictures_get(struct gfx_pictures *pictures, int index);
void gfx_pictures_print(struct gfx_pictures *pictures);
int gfx_pictures_count(struct gfx_pictures *pictures);
void gfx_pictures_destroy(struct gfx_pictures *pictures);

#endif
