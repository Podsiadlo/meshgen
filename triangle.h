#ifndef MESHGEN_TRIANGLE_H
#define MESHGEN_TRIANGLE_H

static const int EPSILON = 100;

#include <stdbool.h>
#include "point.h"
#include "mesh.h"

struct triangle {
    struct point a2;
    struct point b2;
    struct point c2;

    struct triangle *child_ab;
    struct triangle *child_bc;
    struct triangle *child_ac;
    short longest; //0=ab, 1=bc, 2=ac
};

struct triangle *create_triangle(int a_x, int a_y, int b_x, int b_y, int c_x, int c_y, struct mesh *mesh);

void fix_longest(struct triangle *triangle);

int get_height_mean(const struct triangle *triangle);

int get_height_of_center(const struct triangle *triangle, const short **map);

struct triangle *get_next_triangle(struct triangle *triangle);

void get_longest_edge_midsection(struct point *destination, struct triangle *triangle);

#endif //MESHGEN_TRIANGLE_H
