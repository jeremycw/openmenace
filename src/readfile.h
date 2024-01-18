#ifndef READFILE_H
#define READFILE_H

#include <stdint.h>
#include <stdio.h>

#define U32_SIZE 4
#define U16_SIZE 2

uint32_t read_uint32(FILE* file);
uint16_t read_uint16(FILE* file);

#endif
