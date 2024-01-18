#ifndef GFX_DECODER_H
#define GFX_DECODER_H

#include <stdint.h>

struct gfx_decoder;

struct gfx_decoder *gfx_decoder_create(char const *head_path,
                                       char const *graph_path,
                                       char const *dict_path);
void gfx_decoder_destroy(struct gfx_decoder *decoder);
int gfx_decoder_decode_sized_chunk(struct gfx_decoder *decoder, int chunk_id,
                                   uint8_t *dst);
int gfx_decoder_decode_unsized_chunk(struct gfx_decoder *decoder, int chunk_id,
                                     int size, uint8_t *dst);
int gfx_decoder_head_size(struct gfx_decoder *decoder);

#endif // GFX_DECODER_H
