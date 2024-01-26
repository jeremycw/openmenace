#include <stdint.h>
#include <stdlib.h>

#include "gfx_decoder.h"
#include "readfile.h"

struct gfx_sprite {
  uint16_t width_div_8;
  uint16_t height;
  uint16_t x_offset;
  uint16_t y_offset;
  uint16_t clip_rect_left;
  uint16_t clip_rect_top;
  uint16_t clip_rect_right;
  uint16_t clip_rect_bottom;
  uint16_t shift;
};

struct gfx_sprites {
  int n;
  struct gfx_sprite buffer[0];
};

void gfx_sprite_table_deserialize(struct gfx_sprites *sprites, uint8_t *buffer,
                                  int numsprites) {
  for (int i = 0; i < numsprites; i++) {
    sprites->buffer[i].width_div_8 = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].height = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].x_offset = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].y_offset = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].clip_rect_left = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].clip_rect_top = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].clip_rect_right = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].clip_rect_bottom = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprites->buffer[i].shift = *(uint16_t *)buffer;
  }
}

struct gfx_sprites *gfx_sprites_create(struct gfx_decoder *decoder) {
  // decode sprite table chunk
  uint8_t *chunk = gfx_decoder_current_ptr(decoder);
  int size = gfx_decoder_next_chunk(decoder);
  int numsprites = size / (U16_SIZE * 9);

  // alloc sprite table
  struct gfx_sprites *sprites = malloc(sizeof(struct gfx_sprites) +
                                       sizeof(struct gfx_sprite) * numsprites);
  if (sprites == NULL) {
    return NULL;
  }

  gfx_sprite_table_deserialize(sprites, chunk, numsprites);

  return sprites;
}

void gfx_sprites_print(struct gfx_sprites *sprites) {
  for (int i = 0; i < sprites->n; i++) {
    struct gfx_sprite *sprite = &sprites->buffer[i];
    printf("width: %d\n", sprite->width_div_8 * 8);
    printf("height: %d\n", sprite->height);
    printf("x_offset: %d\n", sprite->x_offset);
    printf("y_offset: %d\n", sprite->y_offset);
    printf("clip_rect_left: %d\n", sprite->clip_rect_left);
    printf("clip_rect_top: %d\n", sprite->clip_rect_top);
    printf("clip_rect_right: %d\n", sprite->clip_rect_right);
    printf("clip_rect_bottom: %d\n", sprite->clip_rect_bottom);
    printf("shift: %d\n\n", sprite->shift);
  }
}

void gfx_sprites_destroy(struct gfx_sprites *sprites) { free(sprites); }
