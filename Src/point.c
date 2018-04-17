#include "point.h"

#include <math.h>

void
init_point(struct point *point, double x, double y, const short **map)
{
    point->x = x;
    point->y = y;
    point->z = get_height(x, y, map);
}

double
get_height(double x, double y, const short **map)  //TODO: test it
{
    double z_a = map[(int)floor(x)][(int)floor(y)];
    double z_b = map[(int)ceil(x)][(int)floor(y)];
    double z_c = map[(int)ceil(x)][(int)ceil(y)];
    double z_d = map[(int)floor(x)][(int)ceil(y)];

    double x_fract = x - floor(x);
    double y_fract = y - floor(y);

    double height = 0.;
    height += z_a * x_fract * (1 - y_fract);
    height += z_b * (1 - x_fract) * (1 - y_fract);
    height += z_c * (1 - x_fract) * y_fract;
    height += z_d * x_fract * y_fract;

    return height;
}


bool
point_equals(struct point *a, struct point *b)
{
    return a->x == b->x && a->y == b->y;
}
