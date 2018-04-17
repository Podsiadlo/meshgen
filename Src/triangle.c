#include "triangle.h"

#include <math.h>

void
init_triangle(struct triangle *triangle, double a_x, double a_y, //TODO: Change the way of keeping points in memory
              double b_x, double b_y, double c_x,
              double c_y, const short **map)
{
    init_point(&(triangle->vertices[0]), a_x, a_y, map);
    init_point(&(triangle->vertices[1]), b_x, b_y, map);
    init_point(&(triangle->vertices[2]), c_x, c_y, map);
    fix_longest(triangle);
}

void
fix_longest(struct triangle *triangle) //TODO: get rid of it
{
    double ab = sqrt(pow((triangle->vertices[0].x) -
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
        } else if (fabs(ab - ac) < 0.000001) {
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
    } else if (fabs(ab - bc) < 0.000001) {
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
        } else if (fabs(ac - bc) < 0.000001) {
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

bool
is_in_triangle(double x, double y, const struct triangle *triangle)
{

    double A = 1 / 2 * (-triangle->vertices[1].y * triangle->vertices[2].x +
                        triangle->vertices[0].y * (-triangle->vertices[1].x + triangle->vertices[2].x) +
                        triangle->vertices[0].x * (triangle->vertices[1].y - triangle->vertices[2].y) +
                        triangle->vertices[1].x * triangle->vertices[2].y);
    double sign = A < 0 ? -1 : 1;
    double s = (triangle->vertices[0].y * triangle->vertices[2].x - triangle->vertices[0].x * triangle->vertices[2].y +
                (triangle->vertices[2].y - triangle->vertices[0].y) * x +
                (triangle->vertices[0].x - triangle->vertices[2].x) * y) * sign;
    double t = (triangle->vertices[0].x * triangle->vertices[1].y - triangle->vertices[0].y * triangle->vertices[1].x +
                (triangle->vertices[0].y - triangle->vertices[1].y) * x +
                (triangle->vertices[1].x - triangle->vertices[0].x) * y) * sign;

    return s > 0 && t > 0 && (s + t) < 2 * A * sign;

}

double
get_height_mean(const struct triangle *triangle)
{
    return (triangle->vertices[0].z + triangle->vertices[1].z +
            triangle->vertices[2].z) /
        3;
}

double
get_height_of_center(const struct triangle *triangle, const short **map)
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
