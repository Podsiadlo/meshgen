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
get_height(double x, double y, const double **map)
{
    //using bilinear interpolation
    double upper_left = map[(int)floor(y)][(int)floor(x)];
    double upper_right = map[(int)floor(y)][(int)ceil(x)];
    double lower_right = map[(int)ceil(y)][(int)ceil(x)];
    double lower_left = map[(int)ceil(y)][(int)floor(x)];

    double x_fract = x - floor(x);
    double y_fract = y - floor(y);

    double height = 0.;
    height += upper_left * (1 - x_fract) * (1 - y_fract);
    height += upper_right * x_fract * (1 - y_fract);
    height += lower_right * x_fract * y_fract;
    height += lower_left * (1 - x_fract) * y_fract;

    return height;
}


bool
point_equals(struct point *a, struct point *b)
{
    return fabs(a->x - b->x) < EPSILON && fabs(a->y - b->y) < EPSILON;
}
