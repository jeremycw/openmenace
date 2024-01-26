#include <stdint.h>
#include <stdlib.h>

#include "gfx_decoder.h"
#include "readfile.h"

#define FONT_COUNT 2

struct gfx_font {
  uint16_t line_height;
  uint16_t *char_offsets;
  uint16_t *char_widths;
  uint8_t *data;
};

struct gfx_fonts {
  struct gfx_font buffer[FONT_COUNT];
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

void gfx_fonts_print(struct gfx_fonts *fonts) {
  for (int i = 0; i < FONT_COUNT; i++) {
    struct gfx_font *font = &fonts->buffer[i];
    printf("line_height: %d\n", font->line_height);
    printf("char_offsets: %p\n", font->char_offsets);
    printf("char_widths: %p\n", font->char_widths);
    printf("data: %p\n\n", font->data);
  }
}

struct gfx_fonts *gfx_fonts_create(struct gfx_decoder *decoder) {
  struct gfx_fonts *fonts = malloc(sizeof(struct gfx_fonts));
  if (fonts == NULL) {
    return NULL;
  }

  // decode fonts
  for (int i = 0; i < FONT_COUNT; i++) {
    uint8_t *decoded_chunk = gfx_decoder_current_ptr(decoder);
    gfx_decoder_next_chunk(decoder);
    gfx_font_decode(&fonts->buffer[i], decoded_chunk);
  }

  return fonts;
}

void gfx_fonts_destroy(struct gfx_fonts *fonts) { free(fonts); }
