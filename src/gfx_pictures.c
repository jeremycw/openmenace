#include <stdint.h>
#include <stdlib.h>

#include "gfx_decoder.h"
#include "readfile.h"

#define EGA_8x8_PLANE_SIZE 8
#define EGA_BITS_PER_PIXEL 4
#define UNMASKED_16x16_TILE_SIZE 128
#define MASKED_16x16_TILE_SIZE 160

struct gfx_picture {
  uint8_t *planes[5];
  int width;
  int height;
  uint16_t chunk_id;
};

struct gfx_pictures {
  int n;
  struct gfx_picture buffer[0];
};

void gfx_picture_init(struct gfx_picture *picture, int width, int height,
                      uint16_t chunk_id, int numplanes, uint8_t *ega) {
  picture->width = width;
  picture->height = height;
  picture->chunk_id = chunk_id;
  int plane_size =
      width * height * numplanes * EGA_BITS_PER_PIXEL / BITS_PER_BYTE;
  for (int i = 0; i < numplanes; i++) {
    picture->planes[i] = ega + plane_size * i;
  }
}

struct gfx_pictures *gfx_pictures_create_from_table(uint16_t *table, int n,
                                                    struct gfx_decoder *decoder,
                                                    int numplanes) {
  struct gfx_pictures *pictures =
      malloc(sizeof(struct gfx_pictures) + sizeof(struct gfx_picture) * n);
  if (pictures == NULL) {
    return NULL;
  }

  pictures->n = n;

  // decode pictures
  for (int i = 0; i < n; i++) {
    uint8_t *decoded_chunk = gfx_decoder_current_ptr(decoder);
    gfx_decoder_next_chunk(decoder);
    int chunk_id = gfx_decoder_current_cid(decoder);

    gfx_picture_init(&pictures->buffer[i], table[i * 2], table[i * 2 + 1],
                     chunk_id, numplanes, decoded_chunk);
  }

  return pictures;
}

struct gfx_pictures *
gfx_pictures_create_from_8x8_tile_chunk(struct gfx_decoder *decoder,
                                        int numplanes) {
  int chunk_id = gfx_decoder_current_cid(decoder);
  uint8_t *raw_ega = gfx_decoder_current_ptr(decoder);
  int size = gfx_decoder_next_chunk(decoder);
  int numunmasked8x8 = size / EGA_8x8_PLANE_SIZE * numplanes;

  struct gfx_pictures *pictures =
      malloc(sizeof(struct gfx_pictures) +
             sizeof(struct gfx_picture) * numunmasked8x8);
  if (pictures == NULL) {
    return NULL;
  }

  pictures->n = numunmasked8x8;

  // decode pictures
  for (int i = 0; i < pictures->n; i++) {
    gfx_picture_init(&pictures->buffer[i], 8, 8, chunk_id, numplanes, raw_ega);
    raw_ega += EGA_8x8_PLANE_SIZE * numplanes;
  }

  return pictures;
}

struct gfx_pictures *gfx_pictures_create_from_tiles(struct gfx_decoder *decoder,
                                                    int tile_count,
                                                    int tile_size,
                                                    int numplanes) {
  // alloc unmasked 16x16 tiles

  struct gfx_pictures *pictures = malloc(
      sizeof(struct gfx_pictures) + sizeof(struct gfx_picture) * tile_count);
  if (pictures == NULL) {
    return NULL;
  }

  pictures->n = tile_count;

  for (int i = 0; i < tile_count; i++) {
    uint8_t *raw_ega = gfx_decoder_current_ptr(decoder);
    gfx_decoder_next_unsized_chunk(decoder, UNMASKED_16x16_TILE_SIZE);

    gfx_picture_init(&pictures->buffer[i], tile_size, tile_size, 0, numplanes,
                     raw_ega);
  }

  return pictures;
}

void gfx_pictures_print(struct gfx_pictures *pictures) {
  printf("count: %d\n", pictures->n);
  for (int i = 0; i < pictures->n; i++) {
    struct gfx_picture *picture = &pictures->buffer[i];
    printf("chunk_id: %d\n", picture->chunk_id);
    printf("width: %d\n", picture->width);
    printf("height: %d\n", picture->height);
    printf("planes: %p\n\n", picture->planes);
  }
}

void gfx_pictures_destroy(struct gfx_pictures *pictures) { free(pictures); }
