#include <stdint.h>
#include <stdlib.h>

#include "gfx.h"
#include "bmp_writer.h"
#include "tilesheet.h"

#define BYTES_PER_PIXEL 4
#define WIDTH 1024

void tilesheet_destroy(struct tilesheet *tilesheet) {
  free(tilesheet);
}

struct tilesheet *tilesheet_create(struct gfx *gfx, enum gfx_type picture_type, uint8_t *palette) {
  int iter = 0;
  int tile_width = 0;
  int tile_height = 0;

  struct gfx_picture* picture;

  if (gfx_iterate_pictures(gfx, picture_type, &picture, &iter)) {
    gfx_picture_size(picture, &tile_width, &tile_height);
  } else {
    return NULL;
  }

  if (tile_width != tile_height) {
    return NULL;
  }

  int row_length = WIDTH / tile_width;
  int row_count = gfx_picture_count(gfx, picture_type) / row_length + 1;
  int height = row_count * tile_height;

  struct tilesheet *tilesheet = malloc(sizeof(struct tilesheet) + WIDTH * height * BYTES_PER_PIXEL);
  if (tilesheet == NULL) {
    return NULL;
  }

  struct bmp_writer *bmp_writer = bmp_writer_create(tilesheet->buffer, WIDTH, palette);
  if (bmp_writer == NULL) {
    goto error;
  }

  int count = 0;
  while (gfx_iterate_pictures(gfx, picture_type, &picture, &iter)) {
    bmp_writer_convert_picture(bmp_writer, picture);

    count++;
    int row = count / row_length;
    int col = count % row_length;
    int y = row * tile_height;
    int x = col * tile_width;

    int offset = y * WIDTH * BYTES_PER_PIXEL + x * BYTES_PER_PIXEL;
    bmp_writer_set_buffer(bmp_writer, tilesheet->buffer + offset);
  }

  tilesheet->width = WIDTH;
  tilesheet->height = height;

  bmp_writer_destroy(bmp_writer);

  return tilesheet;

error:
  bmp_writer_destroy(bmp_writer);
  tilesheet_destroy(tilesheet);
  return NULL;
}
