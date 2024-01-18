#include <stdio.h>
#include <stdlib.h>

#include "readfile.h"

#define HUFFMAN_TABLE_NODE_COUNT 255

#define HEAD_EMPTY_SENTINEL 0xffffff
#define HEAD_WORD_SIZE 3

#define KILOBYTE 1024

typedef struct {
  uint16_t bit_0;
  uint16_t bit_1;
} ca_huffnode;

struct gfx_decoder {
  uint32_t *head_buffer;
  int *head_bytes;
  int head_size;
  uint8_t *graph_buffer;
  int graph_size;
  ca_huffnode *dict_buffer;
};

void CAL_HuffExpand(void *src, void *dest, int expLength, ca_huffnode *table,
                    int srcLength) {
  int headptr = 254;
  uint8_t *srcptr = (uint8_t *)src;
  uint8_t *dstptr = (uint8_t *)dest;
  int src_bit = 1; // ch in asm src
  uint8_t src_char = *(srcptr++);
  int len = 0;
  int complen = 1;
  while (len < expLength) {
    if (src_char & src_bit) {
      // We've got a '1' bit.
      headptr = table[headptr].bit_1;
    } else {
      // We've got a '0' bit.
      headptr = table[headptr].bit_0;
    }

    if (headptr > 255) {
      headptr -= 256;
    } else {
      *(dstptr++) = (uint8_t)(headptr & 0xff);
      headptr = 254;
      len++;
      if (len == expLength) {
        break;
      }
    }

    src_bit <<= 1;
    if (src_bit == 256) {
      src_char = *(srcptr++);
      src_bit = 1;
      complen++;
      if (complen > srcLength) {
        break;
      }
    }
  }
}

struct gfx_decoder *gfx_decoder_create(char const *head_path,
                                       char const *graph_path,
                                       char const *dict_path) {
  struct gfx_decoder *decoder = calloc(sizeof(struct gfx_decoder), 1);
  if (decoder == NULL) {
    return NULL;
  }

  FILE *head_file = NULL, *graph_file = NULL, *dict_file = NULL;

  head_file = fopen(head_path, "rb");
  if (head_file == NULL) {
    goto error;
  }

  graph_file = fopen(graph_path, "rb");
  if (graph_file == NULL) {
    goto error;
  }

  dict_file = fopen(dict_path, "rb");
  if (dict_file == NULL) {
    goto error;
  }

  int capacity = KILOBYTE * 32;
  decoder->head_buffer = malloc(capacity);
  if (decoder->head_buffer == NULL) {
    goto error;
  }

  // read in head file
  int i = 0;
  while (!feof(head_file)) {
    uint32_t head = 0;
    fread(&head, HEAD_WORD_SIZE, 1, head_file);
    decoder->head_buffer[i++] = head;
  }
  decoder->head_size = i - 1;
  fclose(head_file);

  // calculate the compressed length of each chunk
  decoder->head_bytes = malloc(sizeof(int) * decoder->head_size);
  if (decoder->head_bytes == NULL) {
    goto error;
  }
  int prev_index = 0;
  for (int i = 1; i < decoder->head_size - 1; i++) {
    if (decoder->head_buffer[i] != HEAD_EMPTY_SENTINEL) {
      decoder->head_bytes[prev_index] =
          decoder->head_buffer[i] - decoder->head_buffer[prev_index];
      prev_index = i;
    }
  }

  // read entire graph file into memory
  decoder->graph_size = decoder->head_buffer[decoder->head_size - 1];
  decoder->graph_buffer = malloc(decoder->graph_size);
  if (decoder->graph_buffer == NULL) {
    goto error;
  }
  fread(decoder->graph_buffer, decoder->graph_size, 1, graph_file);
  fclose(graph_file);

  // read in dictionary
  decoder->dict_buffer = malloc(sizeof(ca_huffnode) * HUFFMAN_TABLE_NODE_COUNT);
  if (decoder->dict_buffer == NULL) {
    goto error;
  }
  for (int i = 0; i < HUFFMAN_TABLE_NODE_COUNT; i++) {
    decoder->dict_buffer[i].bit_0 = read_uint16(dict_file);
    decoder->dict_buffer[i].bit_1 = read_uint16(dict_file);
  }
  fclose(dict_file);

  return decoder;

error:
  fclose(head_file);
  fclose(graph_file);
  fclose(dict_file);
  free(decoder->head_bytes);
  free(decoder->head_buffer);
  free(decoder->graph_buffer);
  free(decoder->dict_buffer);
  return NULL;
}

int gfx_decoder_head_size(struct gfx_decoder *decoder) {
  return decoder->head_size;
}

int gfx_decoder_decode_sized_chunk(struct gfx_decoder *decoder, int chunk_id,
                                   uint8_t *dst) {
  uint32_t offset = decoder->head_buffer[chunk_id];
  uint32_t size = *(uint32_t *)(decoder->graph_buffer + offset);
  uint8_t *compressed_chunk = decoder->graph_buffer + offset + U32_SIZE;
  CAL_HuffExpand(compressed_chunk, dst, size, decoder->dict_buffer,
                 decoder->head_bytes[chunk_id]);
  return size;
}

int gfx_decoder_decode_unsized_chunk(struct gfx_decoder *decoder, int chunk_id,
                                     int size, uint8_t *dst) {
  uint32_t offset = decoder->head_buffer[chunk_id];
  uint8_t *compressed_chunk = decoder->graph_buffer + offset;
  CAL_HuffExpand(compressed_chunk, dst, size, decoder->dict_buffer,
                 decoder->head_bytes[chunk_id]);
  return size;
}

void gfx_decoder_destroy(struct gfx_decoder *decoder) {
  if (decoder == NULL) {
    return;
  }

  free(decoder->head_buffer);
  free(decoder->graph_buffer);
  free(decoder->dict_buffer);
  free(decoder);
}
