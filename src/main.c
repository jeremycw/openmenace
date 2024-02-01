#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "gfx.h"
#include "maps.h"
#include "tilesheet.h"

#define BYTES_PER_PIXEL 4

int main() {
  struct maps *maps = maps_create("data/MAPHEAD.BM1", "data/MAPTEMP.BM1");
  maps_print(maps);
  maps_destroy(maps);

  struct gfx *gfx =
      gfx_create("data/EGAHEAD.BM1", "data/EGAGRAPH.BM1", "data/EGADICT.BM1");
  gfx_print(gfx);

  uint8_t palette[] = {
    0x00, 0x00, 0x00, 0xFF, // Black
    0x00, 0x00, 0xAA, 0xFF, // Blue
    0x00, 0xAA, 0x00, 0xFF, // Green
    0x00, 0xAA, 0xAA, 0xFF, // Cyan
    0xAA, 0x00, 0x00, 0xFF, // Red
    0xAA, 0x00, 0xAA, 0xFF, // Magenta
    0xAA, 0x55, 0x00, 0xFF, // Brown
    0xAA, 0xAA, 0xAA, 0xFF, // Light Gray
    0x55, 0x55, 0x55, 0xFF, // Dark Gray
    0x55, 0x55, 0xFF, 0xFF, // Light Blue
    0x55, 0xFF, 0x55, 0xFF, // Light Green
    0x55, 0xFF, 0xFF, 0xFF, // Light Cyan
    0xFF, 0x55, 0x55, 0xFF, // Light Red
    0xFF, 0x55, 0xFF, 0xFF, // Light Magenta
    0xFF, 0xFF, 0x55, 0xFF, // Yellow
    0xFF, 0xFF, 0xFF, 0xFF, // White
    0x00, 0x00, 0x00, 0x00  // Transparent
  };
  struct tilesheet *tilesheet = tilesheet_create(gfx, GFX_TYPE_UNMASKED_TILE_16, palette);

  stbi_write_png("tilesheet.png", tilesheet->width, tilesheet->height, BYTES_PER_PIXEL, tilesheet->buffer, tilesheet->width * BYTES_PER_PIXEL);

  gfx_destroy(gfx);

  return 0;
}
