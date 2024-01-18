#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"
#include "map.h"

int main() {
  struct map *map = map_create("data/MAPTEMP.BM1");
  map_print(map);
  map_destroy(map);

  struct gfx *gfx =
      gfx_create("data/EGAHEAD.BM1", "data/EGAGRAPH.BM1", "data/EGADICT.BM1");
  gfx_print(gfx);
  gfx_destroy(gfx);

  return 0;
}
