#include "point.h"
#include "triangle.h"
#include "utils.h"

#include <math.h>
#include <values.h>

void
init_point(struct point *point, double x, double y, struct map *map)
{
    point->x = x;
    point->y = y;
    point->z = get_height(x, y, map);
    point->border = 0;
}

double
get_height(double lon, double lat, struct map *map)
{
    //using bilinear interpolation
    double top_left = get_height_wo_interpol(lon, lat, 1, map);
    double top_right = get_height_wo_interpol(lon, lat, 2, map);
    double bottom_right = get_height_wo_interpol(lon, lat, 3, map);
    double bottom_left = get_height_wo_interpol(lon, lat, 4, map);;

    double x_fract = (lon - map->west_border) / map->cell_width -
            floor((lon - map->west_border) / map->cell_width );
    double y_fract = fabs(lat - map->north_border) / map->cell_length -
            floor(fabs(lat - map->north_border) / map->cell_length );

    double height = 0.;
    height += top_left * (1 - x_fract) * (1 - y_fract);
    height += top_right * x_fract * (1 - y_fract);
    height += bottom_right * x_fract * y_fract;
    height += bottom_left * (1 - x_fract) * y_fract;

    return height;
}

//corner: 1 - top_left, 2 - top_right, 3 - bottom_right, 4 - bottom_left
double
get_height_wo_interpol(double lon, double lat, int corner, const struct map *map)
{
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
    if (fun1((map->north_border - lat) / map->cell_length) > map->length - 1) {
        y = (int) (map->length - 1);
    } else if (fun1((map->north_border - lat) / map->cell_length) < 0) {
        y = 0;
    } else {
        y = (int) fun1((map->north_border - lat) / map->cell_length);
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
    return equals(a->x, b->x) && equals(a->y, b->y);
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

struct point *
mul_by_scalar(double scalar, const struct point *point)
{
    struct point * ret_point = (struct point *)malloc(sizeof(struct point));

    ret_point->x = point->x * scalar;
    ret_point->y = point->y * scalar;
    ret_point->z = point->z * scalar;
    return ret_point;
}

void
sort_points(int size, size_t indices[], int coordinate, struct point **points)
{
    for(int i = 0; i<size; ++i) {
        for(int j = 0; j<i; ++j) {
            double j_elem = get_coordinate(coordinate, points[indices[j]]);
            double i_elem = get_coordinate(coordinate, points[indices[i]]);
            if(is_greater(j_elem, i_elem)) {
                size_t tmp = indices[i];
                shift(j, i, indices);
                indices[j] = tmp;
            }
        }
    }
}

