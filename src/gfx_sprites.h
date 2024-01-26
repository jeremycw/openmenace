#ifndef GFX_SPRITES_H
#define GFX_SPRITES_H

struct gfx_sprites;

struct gfx_sprites *gfx_sprites_create(struct gfx_decoder *decoder);
void gfx_sprites_destroy(struct gfx_sprites *sprites);
void gfx_sprites_print(struct gfx_sprites *sprites);

#endif // GFX_SPRITES_H
