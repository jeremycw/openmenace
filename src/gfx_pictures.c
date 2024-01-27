#include <stdint.h>
#include <stdlib.h>

#include "gfx_decoder.h"
#include "readfile.h"

#define EGA_8x8_PLANE_SIZE 8
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

void gfx_pictures_decode(struct gfx_pictures *pictures, uint16_t *picture_table,
                         struct gfx_decoder *decoder, int n, int numplanes) {
  for (int i = 0; i < n; i++) {
    uint8_t *decoded_chunk = gfx_decoder_current_ptr(decoder);
    pictures->buffer[i].chunk_id = gfx_decoder_current_cid(decoder);
    int size = gfx_decoder_next_chunk(decoder);
    pictures->buffer[i].width = picture_table[i * U16_SIZE] * 8;
    pictures->buffer[i].height = picture_table[i * U16_SIZE + 1];
    int plane_size = size / numplanes;
    for (int j = 0; j < numplanes; j++) {
      pictures->buffer[i].planes[j] = decoded_chunk + plane_size * j;
    }
  }
}

void gfx_pictures_deserialize_tiles(struct gfx_pictures *pictures,
                                    uint8_t *raw_ega, int n, int size,
                                    int numplanes, int chunk_id) {
  for (int i = 0; i < n; i++) {
    pictures->buffer[i].chunk_id = chunk_id;
    pictures->buffer[i].width = size;
    pictures->buffer[i].height = size;
    int plane_size = size * size / BITS_PER_BYTE;
    for (int j = 0; j < numplanes; j++) {
      pictures->buffer[i].planes[j] = raw_ega;
      raw_ega += plane_size;
    }
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
  gfx_pictures_decode(pictures, table, decoder, n, numplanes);

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
  gfx_pictures_deserialize_tiles(pictures, raw_ega, numunmasked8x8, 8,
                                 numplanes, chunk_id);

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
    pictures->buffer[i].chunk_id = gfx_decoder_current_cid(decoder);
    pictures->buffer[i].width = tile_size;
    pictures->buffer[i].height = tile_size;
    uint8_t *raw_ega = gfx_decoder_current_ptr(decoder);
    gfx_decoder_next_unsized_chunk(decoder, UNMASKED_16x16_TILE_SIZE);
    int plane_size = tile_size * tile_size / BITS_PER_BYTE;
    for (int j = 0; j < numplanes; j++) {
      pictures->buffer[i].planes[j] = raw_ega + plane_size * j;
    }
  }

  return pictures;
}

void gfx_pictures_print(struct gfx_pictures *pictures) {
  printf("count: %d\n", pictures->n);
  for (int i = 0; i < pictures->n; i++) {
    struct gfx_picture *picture = &pictures->buffer[i];
    printf("width: %d\n", picture->width);
    printf("height: %d\n", picture->height);
    printf("planes: %p\n\n", picture->planes);
  }
}

void gfx_pictures_destroy(struct gfx_pictures *pictures) { free(pictures); }
