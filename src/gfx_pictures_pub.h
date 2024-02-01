#ifndef GFX_PICTURES_PUB_H
#define GFX_PICTURES_PUB_H

struct gfx_picture;

int gfx_picture_palette_index_for_pixel(struct gfx_picture *picture, int pixel_number);
void gfx_picture_size(struct gfx_picture *picture, int *width, int *height);

#endif

