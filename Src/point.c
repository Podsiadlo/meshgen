#include "point.h"
#include "triangle.h"

#include <math.h>

void
init_point(struct point *point, double x, double y, struct map *map)
{
    point->x = x;
    point->y = y;
    point->z = get_height(x, y, map);
}

double
get_height(double x, double y, struct map *map)
{
    //using bilinear interpolation
    double divisor = map->cell_length * map->cell_width;
    double upper_left = map->data[(int)floor(y)][(int)floor(x)];
    double upper_right = map->data[(int)floor(y)][(int)ceil(x)];
    double lower_right = map->data[(int)ceil(y)][(int)ceil(x)];
    double lower_left = map->data[(int)ceil(y)][(int)floor(x)];

    double x_fract = x - floor(x);
    double y_fract = y - floor(y);

    double height = 0.;
    height += upper_left * (1 - x_fract) * (1 - y_fract) / divisor;
    height += upper_right * x_fract * (1 - y_fract) / divisor;
    height += lower_right * x_fract * y_fract / divisor;
    height += lower_left * (1 - x_fract) * y_fract / divisor;

    return height;
}


bool
point_equals(struct point *a, struct point *b)
{
    return fabs(a->x - b->x) < EPSILON && fabs(a->y - b->y) < EPSILON;
}
