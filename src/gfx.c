#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx_common.h"
#include "gfx_decoder.h"
#include "gfx_fonts.h"
#include "gfx_pictures.h"
#include "gfx_sprites.h"
#include "readfile.h"

#define MEGABYTE 1024 * 1024

#define EGA_UNMASKED_PLANE_COUNT 4
#define EGA_MASKED_PLANE_COUNT 5

#define NUM_16x16_UNMASKED 1512
#define NUM_16x16_MASKED 2682

struct gfx {
  struct gfx_pictures *pictures;
  struct gfx_pictures *masked_pictures;
  struct gfx_pictures *unmasked8x8;
  struct gfx_pictures *masked8x8;
  struct gfx_pictures *unmasked16x16;
  struct gfx_pictures *masked16x16;
  struct gfx_sprites *sprites;
  struct gfx_fonts *fonts;
  uint8_t **chunks;
  uint8_t *buffer;
};

void gfx_destroy(struct gfx *gfx) {
  gfx_pictures_destroy(gfx->masked16x16);
  gfx_pictures_destroy(gfx->unmasked16x16);
  gfx_pictures_destroy(gfx->masked8x8);
  gfx_pictures_destroy(gfx->unmasked8x8);
  gfx_pictures_destroy(gfx->masked_pictures);
  gfx_pictures_destroy(gfx->pictures);
  gfx_sprites_destroy(gfx->sprites);
  free(gfx->chunks);
  free(gfx->buffer);
  free(gfx);
}

struct gfx *gfx_create(char const *head_path, char const *graph_path,
                       char const *dict_path) {
  struct gfx *gfx = calloc(sizeof(struct gfx), 1);
  if (gfx == NULL) {
    return NULL;
  }

  struct gfx_decoder *decoder =
      gfx_decoder_create(head_path, graph_path, dict_path);

  // alloc buffer
  gfx->buffer = malloc(MEGABYTE * 4);
  if (gfx->buffer == NULL) {
    goto error;
  }

  // alloc chunks
  gfx->chunks = malloc(sizeof(uint8_t *) * gfx_decoder_head_size(decoder));
  if (gfx->chunks == NULL) {
    goto error;
  }

  gfx_decoder_set_buffers(decoder, gfx->buffer, gfx->chunks);

  int size = 0;

  // decode Picture Table
  uint16_t *picture_table = gfx_decoder_current_ptr(decoder);
  size = gfx_decoder_next_chunk(decoder);
  int numpics = size / (U16_SIZE * 2);

  // decode Masked picture table
  uint16_t *masked_picture_table = gfx_decoder_current_ptr(decoder);
  size = gfx_decoder_next_chunk(decoder);
  int nummaskpics = size / (U16_SIZE * 2);

  gfx->sprites = gfx_sprites_create(decoder);
  if (gfx->sprites == NULL) {
    goto error;
  }

  gfx->fonts = gfx_fonts_create(decoder);
  if (gfx->fonts == NULL) {
    goto error;
  }

  gfx->pictures = gfx_pictures_create_from_table(
      picture_table, numpics, decoder, EGA_UNMASKED_PLANE_COUNT);
  if (gfx->pictures == NULL) {
    goto error;
  }

  gfx->masked_pictures = gfx_pictures_create_from_table(
      masked_picture_table, nummaskpics, decoder, EGA_MASKED_PLANE_COUNT);
  if (gfx->masked_pictures == NULL) {
    goto error;
  }

  gfx->unmasked8x8 = gfx_pictures_create_from_8x8_tile_chunk(
      decoder, EGA_UNMASKED_PLANE_COUNT);
  if (gfx->unmasked8x8 == NULL) {
    goto error;
  }

  gfx->masked8x8 =
      gfx_pictures_create_from_8x8_tile_chunk(decoder, EGA_MASKED_PLANE_COUNT);
  if (gfx->masked8x8 == NULL) {
    goto error;
  }

  gfx->unmasked16x16 = gfx_pictures_create_from_tiles(
      decoder, NUM_16x16_UNMASKED, 16, EGA_UNMASKED_PLANE_COUNT);
  if (gfx->unmasked16x16 == NULL) {
    goto error;
  }

  gfx->masked16x16 = gfx_pictures_create_from_tiles(decoder, NUM_16x16_MASKED,
                                                    16, EGA_MASKED_PLANE_COUNT);
  if (gfx->masked16x16 == NULL) {
    goto error;
  }

  gfx_decoder_destroy(decoder);

  return gfx;

error:
  gfx_decoder_destroy(decoder);
  gfx_destroy(gfx);
  return NULL;
}

int gfx_picture_iter(struct gfx* gfx, enum gfx_picture_type picture_type, int *picture_id, int *iter) {
  struct gfx_pictures* pictures = NULL;

  switch (picture_type) {
  case GFX_PICTURE_TYPE_UNMASKED_PICTURE:
    pictures = gfx->pictures;
    break;
  case GFX_PICTURE_TYPE_MASKED_PICTURE:
    pictures = gfx->masked_pictures;
    break;
  case GFX_PICTURE_TYPE_UNMASKED_TILE_8:
    pictures = gfx->unmasked8x8;
    break;
  case GFX_PICTURE_TYPE_MASKED_TILE_8:
    pictures = gfx->masked8x8;
    break;
  case GFX_PICTURE_TYPE_UNMASKED_TILE_16:
    pictures = gfx->unmasked16x16;
    break;
  case GFX_PICTURE_TYPE_MASKED_TILE_16:
    pictures = gfx->masked16x16;
    break;
  default:
    return 0;
  }

  if (*iter >= gfx_pictures_count(pictures)) {
    return 0;
  }

  *picture_id = *iter;
  *iter += 1;

  return 1;
}

void gfx_print(struct gfx *gfx) {
  printf("Sprites Table:\n\n");
  gfx_sprites_print(gfx->sprites);
  printf("Fonts:\n\n");
  gfx_fonts_print(gfx->fonts);
  printf("Pictures:\n\n");
  gfx_pictures_print(gfx->pictures);
  printf("\nMasked Pictures:\n\n");
  gfx_pictures_print(gfx->masked_pictures);
}
