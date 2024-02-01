#ifndef TILESHEET_H
#define TILESHEET_H

struct tilesheet {
  int width;
  int height;
  uint8_t buffer[0];
};

#define BYTES_PER_PIXEL 4

void tilesheet_destroy(struct tilesheet *tilesheet);
struct tilesheet *tilesheet_create(struct gfx *gfx, enum gfx_type picture_type, uint8_t *palette);

#endif
