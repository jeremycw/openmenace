#ifndef READFILE_H
#define READFILE_H

#include <stdint.h>
#include <stdio.h>

#define U32_SIZE 4
#define U16_SIZE 2
#define BITS_PER_BYTE 8

uint32_t read_uint32(FILE *file);
uint16_t read_uint16(FILE *file);
int32_t read_int32(FILE *file);

#endif
