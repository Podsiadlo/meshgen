#ifndef MESHGEN_TRIANGLE_H
#define MESHGEN_TRIANGLE_H
static const double EPSILON = 0.00001;

#include <stdbool.h>

#include "point.h"

struct triangle {
    int index;
    struct point vertices[3]; // P0, P1, P2
    int neighbours[3]; // 0 -> P0P1, 1 -> P1P2, 2 -> P2P0
    short longest; // 0 -> P0P1, 1 -> P1P2, 2 -> P2P0
};

void
init_triangle(struct triangle *triangle, double a_x, double a_y,
              double b_x, double b_y, double c_x,
              double c_y, struct map *map);

void
fix_longest(struct triangle *triangle);

double
get_area(const struct point *a, const struct point *b, const struct point *c);

void
compute_barycentric_coords(double *barycentric_coords, struct point *point, const struct triangle *triangle);

bool
is_inside_triangle(double x, double y, const struct triangle *triangle);

double
get_height_mean(const struct triangle *triangle);

double
get_height_of_center(const struct triangle *triangle, struct map *map);

int
get_longest_edge_triangle_index(struct triangle *triangle);

int
get_1st_shorter_edge_triangle_index(struct triangle *triangle);

int
get_2nd_shorter_edge_triangle_index(struct triangle *triangle);

void
get_longest_edge_midsection(struct point *destination,
                            struct triangle *triangle);

struct point *
get_opposite_vertex(struct triangle* triangle);

struct point *
get_1st_longest_edge_vertex(struct triangle *triangle);

struct point *
get_2nd_longest_edge_vertex(struct triangle *triangle);

#endif // MESHGEN_TRIANGLE_H
