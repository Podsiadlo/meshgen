#include "point.h"
#include "triangle.h"
//#include <libmgrs/mgrs.h>

#include <math.h>
#include <values.h>

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
    double divisor = 1.00 * 1.00;
    double top_left = map->data[(int)floor(fabs(y - map->north_border)*1200)][(int)floor((x-map->west_border)*1200)];
    double top_right = map->data[(int)floor(fabs(y - map->north_border)*1200)][(int)ceil((x-map->west_border)*1200)];
    double bottom_right = map->data[(int)ceil(fabs(y - map->north_border)*1200)][(int)ceil((x-map->west_border)*1200)];
    double bottom_left = map->data[(int)ceil(fabs(y - map->north_border)*1200)][(int)floor((x-map->west_border)*1200)];

    double x_fract = (x-map->west_border)*1200 - floor((x-map->west_border)*1200);
    double y_fract = fabs(y - map->north_border)*1200 - floor(fabs(y - map->north_border)*1200);

    double height = 0.;
    height += top_left * (1 - x_fract) * (1 - y_fract) /*/ divisor*/;
    height += top_right * x_fract * (1 - y_fract) /*/ divisor*/;
    height += bottom_right * x_fract * y_fract /*/ divisor*/;
    height += bottom_left * (1 - x_fract) * y_fract /*/ divisor*/;

    return height;
}


bool
point_equals(struct point *a, struct point *b)
{
    return fabs(a->x - b->x) < EPSILON && fabs(a->y - b->y) < EPSILON;
}

double
get_coordinate(int coordinate, struct point *point)
{
    switch (coordinate) {
        case 0:
            return point->x;
        case 1:
            return point->y;
        case 2:
            return point->z;
        default:
            return MINDOUBLE;
    }
}
