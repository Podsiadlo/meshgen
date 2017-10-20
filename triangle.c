
#include <math.h>
#include <stdlib.h>
#include "triangle.h"


struct triangle *init_triangle(struct triangle * triangle, unsigned int a_x, unsigned int a_y,
                               unsigned int b_x, unsigned int b_y, unsigned int c_x, unsigned int c_y,
                               const short **map) {
    init_point(&(triangle->vertices[0]), a_x, a_y, map[a_y][a_x]);
    init_point(&(triangle->vertices[1]), b_x, b_y, map[b_y][b_x]);
    init_point(&(triangle->vertices[2]), c_x, c_y, map[c_y][c_x]);
    fix_longest(triangle);
}

void fix_longest(struct triangle *triangle) {
    double ab = sqrt(pow((double)(triangle->vertices[0].x) - (double)(triangle->vertices[1].x), 2)
                     + pow((double)(triangle->vertices[0].y) - (double)(triangle->vertices[1].y), 2));
    double bc = sqrt(pow((double)(triangle->vertices[1].x) - (double)(triangle->vertices[2].x), 2)
                     + pow((double)(triangle->vertices[1].y) - (double)(triangle->vertices[2].y), 2));
    double ac = sqrt(pow((double)(triangle->vertices[2].x) - (double)(triangle->vertices[0].x), 2)
                     + pow((double)(triangle->vertices[2].y) - (double)(triangle->vertices[0].y), 2));

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

int get_height_mean(const struct triangle *triangle) {
    return (triangle->vertices[0].z + triangle->vertices[1].z + triangle->vertices[2].z) / 3;
}

short get_height_of_center(const struct triangle *triangle, const short **map) {
    int x = (triangle->vertices[0].x + triangle->vertices[1].x + triangle->vertices[2].x) / 3;
    int y = (triangle->vertices[0].y + triangle->vertices[1].y + triangle->vertices[2].y) / 3;
    return map[y][x];
}

int get_next_triangle_index(struct triangle *triangle) {
    return triangle->children[triangle->longest];
}

void get_longest_edge_midsection(struct point *destination, struct triangle *triangle) {
    struct point *a, *b;
    if (triangle->longest == 0) {
        a = &(triangle->vertices[0]);
        b = &(triangle->vertices[1]);
    } else if (triangle->longest == 1) {
        a = &(triangle->vertices[2]);
        b = &(triangle->vertices[1]);
    } else {
        a = &(triangle->vertices[0]);
        b = &(triangle->vertices[2]);
    }
    destination->x = (a->x + b->x) / 2;
    destination->y = (a->y + b->y) / 2;
}


void verify_neighbours(struct triangle *triangle, struct mesh *mesh) {
    for (int i = 0; i < 3; ++i) {
        if (triangle->children[i] != -1) {
            struct triangle *neighbour = get_triangle(triangle->children[i], mesh);
            if ((!point_equals(&triangle->vertices[i], &neighbour->vertices[0])
                 && !point_equals(&triangle->vertices[i], &neighbour->vertices[1])
                 && !point_equals(&triangle->vertices[i], &neighbour->vertices[2]))
                || (!point_equals(&triangle->vertices[(i + 1) % 3], &neighbour->vertices[0])
                    && !point_equals(&triangle->vertices[(i + 1) % 3], &neighbour->vertices[1])
                    && !point_equals(&triangle->vertices[(i + 1) % 3], &neighbour->vertices[2]))) {

                exit(4);
            }
            if (neighbour->children[0] != triangle->index
                && neighbour->children[1] != triangle->index
                && neighbour->children[2] != triangle->index) {

                exit(4);
            }
        }
    }
}