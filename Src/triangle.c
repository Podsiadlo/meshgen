#include "triangle.h"

#include <math.h>

void
init_triangle(struct triangle *triangle, double a_x, double a_y, //TODO: Change the way of keeping points in memory
              double b_x, double b_y, double c_x,
              double c_y, struct map *map)
{
    init_point(&(triangle->vertices[0]), a_x, a_y, map);
    init_point(&(triangle->vertices[1]), b_x, b_y, map);
    init_point(&(triangle->vertices[2]), c_x, c_y, map);
    fix_longest(triangle);
}

void
fix_longest(struct triangle *triangle)
{
    double ab = sqrt(pow((triangle->vertices[0].x) - //TODO: Do I need to take sqrt of it?
                             (triangle->vertices[1].x),
                         2) +
                     pow((triangle->vertices[0].y) -
                             (triangle->vertices[1].y),
                         2));
    double bc = sqrt(pow((triangle->vertices[1].x) -
                             (triangle->vertices[2].x),
                         2) +
                     pow((triangle->vertices[1].y) -
                             (triangle->vertices[2].y),
                         2));
    double ac = sqrt(pow((triangle->vertices[2].x) -
                             (triangle->vertices[0].x),
                         2) +
                     pow((triangle->vertices[2].y) -
                             (triangle->vertices[0].y),
                         2));

    short longest;
    if (ab > bc) {
        if (ab > ac) {
            longest = 0;
        } else if (fabs(ab - ac) < EPSILON) {
            if (triangle->vertices[1].y > triangle->vertices[2].y) {
                longest = 2;
            } else if (triangle->vertices[1].y < triangle->vertices[2].y) {
                longest = 0;
            } else {
                if (triangle->vertices[1].x > triangle->vertices[2].x) {
                    longest = 2;
                } else {
                    longest = 0;
                }
            }
        } else {
            longest = 2;
        }
    } else if (fabs(ab - bc) < EPSILON) {
        if (ab >= ac) {
            if (triangle->vertices[0].y > triangle->vertices[2].y) {
                longest = 1;
            } else if (triangle->vertices[0].y < triangle->vertices[2].y) {
                longest = 0;
            } else {
                if (triangle->vertices[0].x > triangle->vertices[2].x) {
                    longest = 1;
                } else {
                    longest = 0;
                }
            }
        } else {
            longest = 2;
        }
    } else {
        if (bc > ac) {
            longest = 1;
        } else if (fabs(ac - bc) < EPSILON) {
            if (triangle->vertices[1].y > triangle->vertices[0].y) {
                longest = 2;
            } else if (triangle->vertices[1].y < triangle->vertices[0].y) {
                longest = 1;
            } else {
                if (triangle->vertices[1].x > triangle->vertices[0].x) {
                    longest = 2;
                } else {
                    longest = 1;
                }
            }
        } else {
            longest = 2;
        }
    }
    triangle->longest = longest;
}

double
get_area(const struct point *a, const struct point *b, const struct point *c)
{
    return 0.5 * ((b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x));
}

void
compute_barycentric_coords(double *barycentric_coords, struct point *point, const struct triangle *triangle) {
    double area_triangle = get_area(&triangle->vertices[0], &triangle->vertices[1], &triangle->vertices[2]);
    barycentric_coords[2] = get_area(point, &triangle->vertices[0], &triangle->vertices[1]) / area_triangle;
    barycentric_coords[1]= get_area(point, &triangle->vertices[2], &triangle->vertices[0]) / area_triangle;
    barycentric_coords[0] = get_area(point, &triangle->vertices[1], &triangle->vertices[2]) / area_triangle;
}

bool
is_inside_triangle(double x, double y, const struct triangle *triangle)
{
    double x1 = triangle->vertices[0].x;
    double y1 = triangle->vertices[0].y;
    double x2 = triangle->vertices[1].x;
    double y2 = triangle->vertices[1].y;
    double x3 = triangle->vertices[2].x;
    double y3 = triangle->vertices[2].y;

    double denominator = ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3)); //TODO: can be moved out as it is constant for a triangle
    double a = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denominator;
    double b = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denominator;
    double c = 1 - a - b;

    return -EPSILON <= a && a <= 1 + EPSILON && -EPSILON <= b && b <= 1 + EPSILON && -EPSILON <= c && c <= 1 + EPSILON;
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
