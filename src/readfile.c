#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "readfile.h"

uint32_t read_uint32(FILE *file) {
  uint32_t value;
  fread(&value, sizeof(uint32_t), 1, file);
  return value;
}

int32_t read_int32(FILE *file) {
  int32_t value;
  fread(&value, sizeof(int32_t), 1, file);
  return value;
}

uint16_t read_uint16(FILE *file) {
  uint16_t value;
  fread(&value, sizeof(uint16_t), 1, file);
  return value;
}
