#include "point.h"
#include "triangle.h"
#include "utils.h"
#include "libmgrs/utm.h"
#include "input.h"

#include <math.h>
#include <values.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

void
init_point(struct point *point, double x, double y, bool utm, struct map *map)
{
    if (!utm) {
        Convert_Geodetic_To_UTM(d2r(y), d2r(x), &(map->zone), &(map->hemisphere), &(point->x), &(point->y));
        point->z = get_height(x, y, map);
    } else {
        point->x = x;
        point->y = y;
        point->z = get_point_height(point, map);
    }
}

double
get_point_height(struct point *point, struct map *map)
{
    double lat;
    double lon;
    if (Convert_UTM_To_Geodetic(map->zone, map->hemisphere, point->x, point->y, &lat, &lon)) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    return get_height(r2d(lon), r2d(lat), map);
}

double
get_height(double lon, double lat, struct map *map)
{
    //using bilinear interpolation
//    double divisor = 1.00 * 1.00;
    double top_left = get_height_wo_interpol(lon, lat, 1, map);
    double top_right = get_height_wo_interpol(lon, lat, 2, map);
    double bottom_right = get_height_wo_interpol(lon, lat, 3, map);
    double bottom_left = get_height_wo_interpol(lon, lat, 4, map);;

    double x_fract = (lon - map->west_border) / map->cell_width -
            floor((lon - map->west_border) / map->cell_width );
    double y_fract = fabs(lat - map->north_border) / map->cell_length -
            floor(fabs(lat - map->north_border) / map->cell_length );

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
