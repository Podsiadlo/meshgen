#include "point.h"
#include "triangle.h"
//#include "utils.h"
//#include "libmgrs/utm.h"
#include "input.h"

#include <math.h>
#include <values.h>
//#include <errno.h>
//#include <string.h>
//#include <stdio.h>

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

//corner: 1 - top_left, 2 - top_right, 3 - bottom_right, 4 - bootom_left
double get_height_wo_interpol(double lon, double lat, int corner, const struct map *map) {
    double (*fun1)(double);
    double (*fun2)(double);
    switch (corner) {
        case 1:
            fun1 = floor;
            fun2 = floor;
            break;
        case 2:
            fun1 = floor;
            fun2 = ceil;
            break;
        case 3:
            fun1 = ceil;
            fun2 = ceil;
            break;
        case 4:
            fun1 = ceil;
            fun2 = floor;
            break;
        default:
            return MINDOUBLE;
    }

    int y, x;
    if (fun1((lat - map->north_border) / map->cell_length) > map->length - 1) {
        y = (int) (map->length - 1);
    } else if (fun1((lat - map->north_border) / map->cell_length) < 0) {
        y = 0;
    } else {
        y = (int) fun1((lat - map->north_border) / map->cell_length);
    }
    if (fun2((lon - map->west_border) / map->cell_width) > map->width - 1) {
        x = (int) (map->width - 1);
    } else if (fun2((lon - map->west_border) / map->cell_width) < 0) {
        x = 0;
    } else {
        x = (int) fun2((lon - map->west_border) / map->cell_width);
    }
    return map->data[y][x];
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
