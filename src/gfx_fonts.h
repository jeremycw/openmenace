#ifndef GFX_FONTS_H
#define GFX_FONTS_H

struct gfx_fonts *gfx_fonts_create(struct gfx_decoder *decoder);
void gfx_fonts_print(struct gfx_fonts *fonts);
void gfx_fonts_destroy(struct gfx_fonts *fonts);

#endif
