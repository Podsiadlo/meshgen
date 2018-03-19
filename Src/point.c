#include "point.h"

#include <stdbool.h>

void
init_point(struct point *point, unsigned int x, unsigned int y, short z)
{
    point->x = x;
    point->y = y;
    point->z = z;
}

bool
point_equals(struct point *a, struct point *b)
{
    return a->x == b->x && a->y == b->y;
}
