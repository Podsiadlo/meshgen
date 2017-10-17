#ifndef MESHGEN_TRIANGLE_H
#define MESHGEN_TRIANGLE_H

static const int EPSILON = 7;

#include <stdbool.h>
#include "point.h"
#include "mesh.h"

struct triangle {
    struct point a;
    struct point b;
    struct point c;

    int index;
    short longest; //0=ab, 1=bc, 2=ac

    int child_ab;
    int child_bc;
    int child_ac;
};

struct triangle *init_triangle(struct triangle * triangle, int a_x, int a_y, int b_x, int b_y, int c_x, int c_y,
                               const short **map);

void fix_longest(struct triangle *triangle);

int get_height_mean(const struct triangle *triangle);

int get_height_of_center(const struct triangle *triangle, const short **map);

int get_next_triangle_index(struct triangle *triangle);

void get_longest_edge_midsection(struct point *destination, struct triangle *triangle);

#endif //MESHGEN_TRIANGLE_H
