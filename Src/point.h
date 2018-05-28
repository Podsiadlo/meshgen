#ifndef MESHGEN_POINT_H
#define MESHGEN_POINT_H

#include <stdbool.h>
#include "map.h"

struct point {
    double x;
    double y;
    double z;
};

void
init_point(struct point *point, double x, double y, bool utm, struct map *map);

double
get_point_height(struct point *point, struct map *map);

double
get_height(double lon, double lat, struct map *map);

bool
point_equals(struct point *a, struct point *b);

double
get_coordinate(int coordinate, struct point *point);

#endif // MESHGEN_POINT_H
