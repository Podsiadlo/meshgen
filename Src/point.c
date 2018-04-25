#include "point.h"
#include "triangle.h"

#include <math.h>

void
init_point(struct point *point, double x, double y, const double **map)
{
    point->x = x;
    point->y = y;
    point->z = get_height(x, y, map);
}

double
get_height(double x, double y, const double **map)  //TODO: test it
{
    double z_a = map[(int)floor(y)][(int)floor(x)];
    double z_b = map[(int)floor(y)][(int)ceil(x)];
    double z_c = map[(int)ceil(y)][(int)ceil(x)];
    double z_d = map[(int)ceil(y)][(int)floor(x)];

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
    return fabs(a->x - b->x) < EPSILON && fabs(a->y - b->y) < EPSILON;
}
