#ifndef BMP_WRITER_H
#define BMP_WRITER_H

struct bmp_writer;
struct gfx_picture;

struct bmp_writer *bmp_writer_create(uint8_t *buffer, int stride, uint8_t *palette);
void bmp_writer_convert_picture(struct bmp_writer *bmp_writer, struct gfx_picture* picture);
void bmp_writer_set_buffer(struct bmp_writer *bmp_writer, uint8_t *buffer);
void bmp_writer_destroy(struct bmp_writer *bmp_writer);

#endif
