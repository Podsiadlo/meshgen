#include <stdlib.h>
#include <stdbool.h>
#include "point.h"


struct point *create_point(int x, int y) {
    return (struct point *) malloc(sizeof(struct point));
}

void init_point(struct point * point, int x, int y, int z) {
    point->x = x;
    point->y = y;
    point->z = z;
}

bool point_equals(struct point *a, struct point *b) {
    return a->x == b->x && a->y == b->y;
}
