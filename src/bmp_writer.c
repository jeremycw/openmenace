#include <stdint.h>
#include <stdlib.h>

#include "gfx.h"

#define BYTES_PER_PIXEL 4

struct bmp_writer {
  uint8_t *buffer;
  int stride;
  uint8_t *palette;
};

struct bmp_writer *bmp_writer_create(uint8_t *buffer, int stride, uint8_t *palette) {
  struct bmp_writer *bmp_writer = malloc(sizeof(struct bmp_writer));
  if (bmp_writer == NULL) {
    return NULL;
  }

  bmp_writer->buffer = buffer;
  bmp_writer->stride = stride;
  bmp_writer->palette = palette;

  return bmp_writer;
}

void bmp_writer_convert_picture(struct bmp_writer *bmp_writer, struct gfx_picture* picture) {
  int width = 0;
  int height = 0;
  gfx_picture_size(picture, &width, &height);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int palette_pixel_index = gfx_picture_palette_index_for_pixel(picture, y * width + x);
      int dst_pixel_index = y * bmp_writer->stride + x;
      for (int i = 0; i < BYTES_PER_PIXEL; ++i) {
        int dst_colour_component_index = dst_pixel_index * BYTES_PER_PIXEL + i;
        int palette_colour_component_index = palette_pixel_index * BYTES_PER_PIXEL + i;
        bmp_writer->buffer[dst_colour_component_index] = bmp_writer->palette[palette_colour_component_index];
      }
    }
  }
}

void bmp_writer_set_buffer(struct bmp_writer *bmp_writer, uint8_t *buffer) {
  bmp_writer->buffer = buffer;
}

void bmp_writer_destroy(struct bmp_writer *bmp_writer) {
  free(bmp_writer);
}
