#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx_decoder.h"
#include "readfile.h"

#define MEGABYTE 1024 * 1024

#define FONT_START_CHUNK_ID 3
#define FONT_COUNT 2

#define EGA_PLANE_COUNT 4
#define EGA_MASKED_PLANE_COUNT 5

struct gfx_font {
  uint16_t line_height;
  uint16_t *char_offsets;
  uint16_t *char_widths;
  uint8_t *data;
};

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

struct gfx_picture {
  int width;
  int height;
  uint8_t* planes[5];
};

struct gfx_decoder_iter {
  int cid;
  uint8_t *buf;
};

struct gfx {
  struct gfx_picture *pictures;
  struct gfx_picture *masked_pictures;
  uint16_t *picture_table;
  int numpics;
  uint16_t *masked_picture_table;
  int nummaskpics;
  struct gfx_sprite *sprite_table;
  int numsprites;
  struct gfx_font fonts[FONT_COUNT];
  uint8_t **chunks;
  uint8_t *buffer;
};

void gfx_font_decode(struct gfx_font *font, uint8_t *buffer) {
  font->line_height = *(uint16_t *)buffer;
  buffer += U16_SIZE;
  font->char_offsets = (uint16_t *)buffer;
  buffer += 256 * U16_SIZE;
  font->char_widths = (uint16_t *)buffer;
  buffer += 256 * U16_SIZE;
  font->data = buffer;
}

void gfx_sprite_table_deserialize(struct gfx_sprite *sprite_table, uint8_t *buffer,
                                  int numsprites) {
  for (int i = 0; i < numsprites; i++) {
    sprite_table[i].width_div_8 = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].height = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].x_offset = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].y_offset = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].clip_rect_left = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].clip_rect_top = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].clip_rect_right = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].clip_rect_bottom = *(uint16_t *)buffer;
    buffer += U16_SIZE;
    sprite_table[i].shift = *(uint16_t *)buffer;
  }
}

void gfx_sprite_table_print(struct gfx_sprite *sprite_table, int numsprites) {
  printf("Sprites Table:\n\n");
  for (int i = 0; i < numsprites; i++) {
    struct gfx_sprite *sprite = &sprite_table[i];
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

void gfx_fonts_print(struct gfx_font *fonts, int numfonts) {
  printf("Fonts:\n\n");
  for (int i = 0; i < numfonts; i++) {
    struct gfx_font *font = &fonts[i];
    printf("line_height: %d\n", font->line_height);
    printf("char_offsets: %p\n", font->char_offsets);
    printf("char_widths: %p\n", font->char_widths);
    printf("data: %p\n\n", font->data);
  }
}


int gfx_decode_next_chunk(struct gfx *gfx,
                          struct gfx_decoder_iter *iter,
                          struct gfx_decoder *decoder) {
  gfx->chunks[iter->cid] = iter->buf;
  int size = gfx_decoder_decode_sized_chunk(decoder, iter->cid, iter->buf);
  iter->buf += size;
  iter->cid++;
  return size;
}

void gfx_decode_pictures(struct gfx* gfx, struct gfx_picture* pictures, uint16_t* picture_table, struct gfx_decoder_iter* iter, struct gfx_decoder *decoder, int n, int numplanes) {
  for (int i = 0; i < n; i++) {
    uint8_t *decoded_chunk = iter->buf;
    int size = gfx_decode_next_chunk(gfx, iter, decoder);
    pictures[i].width = picture_table[i*U16_SIZE] * 8;
    pictures[i].height = picture_table[i*U16_SIZE+1];
    int plane_size = size / numplanes;
    for (int j = 0; j < numplanes; j++) {
      pictures[i].planes[j] = decoded_chunk + plane_size * j;
    }
  }
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

  int size = 0;
  struct gfx_decoder_iter iter = {0, gfx->buffer};

  // decode Picture Table
  gfx->picture_table = (uint16_t *)iter.buf;
  size = gfx_decode_next_chunk(gfx, &iter, decoder);
  gfx->numpics = size / (U16_SIZE * 2);

  // decode Masked picture table
  gfx->masked_picture_table = (uint16_t *)iter.buf;
  size = gfx_decode_next_chunk(gfx, &iter, decoder);
  gfx->nummaskpics = size / (U16_SIZE * 2);

  // decode sprite table chunk
  uint8_t* sprite_chunk = iter.buf;
  size = gfx_decode_next_chunk(gfx, &iter, decoder);
  gfx->numsprites = size / (U16_SIZE * 9);

  // alloc sprite table
  gfx->sprite_table = malloc(sizeof(struct gfx_sprite) * gfx->numsprites);
  if (gfx->sprite_table == NULL) {
    goto error;
  }

  // deserialize sprite table
  gfx_sprite_table_deserialize(gfx->sprite_table, sprite_chunk, gfx->numsprites);

  // decode fonts
  for (int i = 0; i < FONT_COUNT; i++) {
    uint8_t *decoded_chunk = iter.buf;
    gfx_decode_next_chunk(gfx, &iter, decoder);
    gfx_font_decode(&gfx->fonts[i], decoded_chunk);
  }

  // alloc pictures
  gfx->pictures = malloc(sizeof(struct gfx_picture) * gfx->numpics);
  if (gfx->pictures == NULL) {
    goto error;
  }

  // decode pictures
  gfx_decode_pictures(gfx, gfx->pictures, gfx->picture_table, &iter, decoder,
      gfx->numpics, EGA_PLANE_COUNT);

  // alloc masked pictures
  gfx->masked_pictures = malloc(sizeof(struct gfx_picture) * gfx->nummaskpics);
  if (gfx->pictures == NULL) {
    goto error;
  }

  // decode masked pictures
  gfx_decode_pictures(gfx, gfx->masked_pictures, gfx->masked_picture_table,
      &iter, decoder, gfx->nummaskpics, EGA_MASKED_PLANE_COUNT);

  gfx_decoder_destroy(decoder);

  return gfx;

error:
  gfx_decoder_destroy(decoder);
  free(gfx->masked_pictures);
  free(gfx->pictures);
  free(gfx->sprite_table);
  free(gfx->chunks);
  free(gfx->buffer);
  free(gfx);
  return NULL;
}

void gfx_print(struct gfx *gfx) {
  printf("numpics: %d\n", gfx->numpics);
  printf("nummaskpics: %d\n", gfx->nummaskpics);
  printf("numsprites: %d\n", gfx->numsprites);
  gfx_sprite_table_print(gfx->sprite_table, gfx->numsprites);
  gfx_fonts_print(gfx->fonts, FONT_COUNT);
}

void gfx_destroy(struct gfx *gfx) {
  free(gfx->buffer);
  free(gfx->chunks);
  free(gfx);
}
