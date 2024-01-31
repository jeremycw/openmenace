#ifndef GFX_COMMON_H
#define GFX_COMMON_H

#include <stdint.h>

struct gfx_ega_graphic {
  uint8_t const *planes[5];
  int width;
  int height;
};

#endif
