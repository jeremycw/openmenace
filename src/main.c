#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"
#include "maps.h"

int main() {
  struct maps *maps = maps_create("data/MAPHEAD.BM1", "data/MAPTEMP.BM1");
  maps_print(maps);
  maps_destroy(maps);

  struct gfx *gfx =
      gfx_create("data/EGAHEAD.BM1", "data/EGAGRAPH.BM1", "data/EGADICT.BM1");
  gfx_print(gfx);
  gfx_destroy(gfx);

  return 0;
}
