#include "triangle.h"
#include "../Utils/utils.h"
#include "../libmgrs/utm.h"

#include <math.h>
#include <stdio.h>

void
init_triangle(double a_x, double a_y, double b_x, double b_y, double c_x, double c_y, struct triangle *triangle,
              struct map *map, bool use_height)
{
    init_point(&(triangle->vertices[0]), a_x, a_y, map);
    init_point(&(triangle->vertices[1]), b_x, b_y, map);
    init_point(&(triangle->vertices[2]), c_x, c_y, map);
    fix_longest(triangle, use_height);
}


void
convert_triangle_to_UTM(struct triangle *triangle, long *zone, char *hemisphere) {
    double x, y;
    for (int i = 0; i < 3; ++i) {
        if (Convert_Geodetic_To_UTM(d2r(triangle->vertices[i].y), d2r(triangle->vertices[i].x),
                                    zone, hemisphere, &x, &y)) {
            fprintf(stderr, "Error during conversion.\n");
            exit(13);
        }
        triangle->vertices[i].x = x;
        triangle->vertices[i].y = y;
    }
}

void
fix_longest(struct triangle *triangle, bool use_height)
{
    double ab, bc, ac;
    if (use_height) {
        ab = pow(triangle->vertices[0].x - triangle->vertices[1].x, 2)
                  + pow(triangle->vertices[0].y - triangle->vertices[1].y, 2)
                  + pow(triangle->vertices[0].z - triangle->vertices[1].z, 2);
        bc = pow(triangle->vertices[1].x - triangle->vertices[2].x, 2)
                  + pow(triangle->vertices[1].y - triangle->vertices[2].y, 2)
                  + pow(triangle->vertices[1].z - triangle->vertices[2].z, 2);
        ac = pow(triangle->vertices[2].x - triangle->vertices[0].x, 2)
                  + pow(triangle->vertices[2].y - triangle->vertices[0].y, 2)
                  + pow(triangle->vertices[2].z - triangle->vertices[0].z, 2);
    } else {
        ab = pow((triangle->vertices[0].x) - (triangle->vertices[1].x), 2)
                + pow((triangle->vertices[0].y) - (triangle->vertices[1].y), 2);
        bc = pow((triangle->vertices[1].x) - (triangle->vertices[2].x), 2)
                + pow((triangle->vertices[1].y) - (triangle->vertices[2].y), 2);
        ac = pow((triangle->vertices[2].x) - (triangle->vertices[0].x), 2)
                + pow((triangle->vertices[2].y) - (triangle->vertices[0].y), 2);
    }

    triangle->longest = choose_longest(ab, bc, ac, triangle);
}

short choose_longest(double ab, double bc, double ac, const struct triangle *triangle) {
    short longest;
    if (is_greater(ab, bc)) {
        if (is_greater(ab, ac)) {
            longest = 0;
        } else if (equals(ab, ac)) {
            if (is_greater(triangle->vertices[1].y, triangle->vertices[2].y)) {
                longest = 2;
            } else if (is_lesser(triangle->vertices[1].y, triangle->vertices[2].y)) {
                longest = 0;
            } else {
                if (is_greater(triangle->vertices[1].x, triangle->vertices[2].x)) {
                    longest = 2;
                } else {
                    longest = 0;
                }
            }
        } else {
            longest = 2;
        }
    } else if (equals(ab, bc)) {
        if (!is_lesser(ab, ac)) {
            if (is_greater(triangle->vertices[0].y, triangle->vertices[2].y)) {
                longest = 1;
            } else if (is_lesser(triangle->vertices[0].y, triangle->vertices[2].y)) {
                longest = 0;
            } else {
                if (is_greater(triangle->vertices[0].x, triangle->vertices[2].x)) {
                    longest = 1;
                } else {
                    longest = 0;
                }
            }
        } else {
            longest = 2;
        }
    } else {
        if (is_greater(bc, ac)) {
            longest = 1;
        } else if (equals(ac, bc)) {
            if (is_greater(triangle->vertices[1].y, triangle->vertices[0].y)) {
                longest = 2;
            } else if (is_lesser(triangle->vertices[1].y, triangle->vertices[0].y)) {
                longest = 1;
            } else {
                if (is_greater(triangle->vertices[1].x, triangle->vertices[0].x)) {
                    longest = 2;
                } else {
                    longest = 1;
                }
            }
        } else {
            longest = 2;
        }
    }
    return longest;
}

double
get_area(const struct point *a, const struct point *b, const struct point *c)
{
    return 0.5 * fabs((b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x));
}

void
compute_barycentric_coords(double *barycentric_coords, struct point *point, const struct triangle *triangle) {
    double triangle_area = get_area(&triangle->vertices[0], &triangle->vertices[1], &triangle->vertices[2]);
    barycentric_coords[2] = get_area(point, &triangle->vertices[0], &triangle->vertices[1]) / triangle_area;
    barycentric_coords[1]= get_area(point, &triangle->vertices[2], &triangle->vertices[0]) / triangle_area;
    barycentric_coords[0] = get_area(point, &triangle->vertices[1], &triangle->vertices[2]) / triangle_area;
}

bool
is_inside_triangle(double barycentric_coords[])
{
    return !is_greater(barycentric_coords[0] + barycentric_coords[1] + barycentric_coords[2], 1.);
}

double
get_height_mean(const struct triangle *triangle)
{
    return (triangle->vertices[0].z + triangle->vertices[1].z +
            triangle->vertices[2].z) /
        3;
}

double
get_height_of_center(const struct triangle *triangle, struct map *map)
{
    double x = (triangle->vertices[0].x + triangle->vertices[1].x +
             triangle->vertices[2].x) /
        3;
    double y = (triangle->vertices[0].y + triangle->vertices[1].y +
             triangle->vertices[2].y) /
        3;
    return get_height(x, y, map);
}

int
get_longest_edge_triangle_index(struct triangle *triangle)
{
    return triangle->neighbours[triangle->longest];
}

int
get_1st_shorter_edge_triangle_index(struct triangle *triangle)
{
    return triangle->neighbours[(triangle->longest + 1) % 3];
}

int
get_2nd_shorter_edge_triangle_index(struct triangle *triangle)
{
    return triangle->neighbours[(triangle->longest + 2) % 3];
}

void
get_longest_edge_midsection(struct point *destination,
                            struct triangle *triangle)
{
    struct point *a, *b;
    a = &(triangle->vertices[triangle->longest]);
    b = &(triangle->vertices[(triangle->longest + 1) % 3]);
    destination->x = (a->x + b->x) / 2;
    destination->y = (a->y + b->y) / 2;
    destination->border = a->border & b->border;
}

struct point *
get_opposite_vertex(struct triangle* triangle)
{
    return &triangle->vertices[(triangle->longest + 2) % 3];
}

struct point *
get_1st_longest_edge_vertex(struct triangle *triangle)
{
    return &triangle->vertices[triangle->longest];
}

struct point *
get_2nd_longest_edge_vertex(struct triangle *triangle)
{
    return &triangle->vertices[(triangle->longest + 1) % 3];
}
