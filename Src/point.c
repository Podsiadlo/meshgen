#include "point.h"
#include "triangle.h"
#include "utils.h"
#include "libmgrs/utm.h"

#include <math.h>
#include <values.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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
    int VALUES_IN_DEGREE = 1200;
    double top_left = map->data
            [(int) fmax(floor((map->north_border - lat) * VALUES_IN_DEGREE), map->length - 1)]
            [(int) fmax(floor((lon - map->west_border) * VALUES_IN_DEGREE), map->width - 1)];
    double top_right = map->data
            [(int) fmax(floor((map->north_border - lat) * VALUES_IN_DEGREE), map->length - 1)]
            [(int) fmax(ceil((lon - map->west_border) * VALUES_IN_DEGREE), map->width - 1)];
    double bottom_right = map->data
            [(int) fmax(ceil((map->north_border - lat) * VALUES_IN_DEGREE), map->length - 1)]
            [(int) fmax(ceil((lon - map->west_border) * VALUES_IN_DEGREE), map->width - 1)];
    double bottom_left = map->data
            [(int) fmax(ceil((map->north_border - lat) * VALUES_IN_DEGREE), map->length - 1)]
            [(int) fmax(floor((lon - map->west_border) * VALUES_IN_DEGREE), map->width - 1)];

    double x_fract = (lon - map->west_border) * VALUES_IN_DEGREE -
            floor((lon - map->west_border) * VALUES_IN_DEGREE);
    double y_fract = fabs(lat - map->north_border) * VALUES_IN_DEGREE -
            floor(fabs(lat - map->north_border) * VALUES_IN_DEGREE);

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
